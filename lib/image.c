#include "image.h"
#include "data.h"
#include "calculations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ============================================================================
// Color Helpers
// ============================================================================

static Color get_space_color(SpaceType type) {
  switch (type) {
    case Standard: return COLOR_STANDARD;
    case Handicap: return COLOR_HANDICAP;
    case Compact:  return COLOR_COMPACT;
    case EV:       return COLOR_EV;
    default:       return COLOR_STANDARD;
  }
}

static Color get_pixel(Color *buffer, int img_width, int img_height, int x, int y) {
  if (x >= 0 && x < img_width && y >= 0 && y < img_height) {
    return buffer[y * img_width + x];
  }
  return COLOR_BACKGROUND;
}

static Color blend_colors(Color bg, Color fg, double alpha) {
  if (alpha < 0.0) alpha = 0.0;
  if (alpha > 1.0) alpha = 1.0;

  return (Color){
    .r = (unsigned char)(bg.r * (1.0 - alpha) + fg.r * alpha),
    .g = (unsigned char)(bg.g * (1.0 - alpha) + fg.g * alpha),
    .b = (unsigned char)(bg.b * (1.0 - alpha) + fg.b * alpha)
  };
}

static void set_pixel(Color *buffer, int img_width, int img_height, int x, int y, Color color) {
  if (x >= 0 && x < img_width && y >= 0 && y < img_height) {
    buffer[y * img_width + x] = color;
  }
}

static void set_pixel_alpha(Color *buffer, int img_width, int img_height,
                            int x, int y, Color color, double alpha) {
  if (x >= 0 && x < img_width && y >= 0 && y < img_height) {
    Color bg = get_pixel(buffer, img_width, img_height, x, y);
    set_pixel(buffer, img_width, img_height, x, y, blend_colors(bg, color, alpha));
  }
}

// ============================================================================
// Vector Math Helpers
// ============================================================================

static double point_to_rect_edge_distance(const Rectangle *rect, Vector point) {
  double min_dist = 1e9;

  for (int i = 0; i < 4; i++) {
    int j = (i + 1) % 4;
    double dist = point_to_segment_distance(point, rect->corner[i], rect->corner[j]);
    if (dist < min_dist) {
      min_dist = dist;
    }
  }
  return min_dist;
}

// ============================================================================
// Wu's Algorithm Helpers
// ============================================================================

static double fpart(double x) {
  return x - floor(x);
}

static double rfpart(double x) {
  return 1.0 - fpart(x);
}

// ============================================================================
// Drawing Functions
// ============================================================================

void draw_line(Color *buffer, int img_width, int img_height,
               double x0, double y0, double x1, double y1,
               Color color, int thickness) {
  if (thickness > 1) {
    Vector dir = { x1 - x0, y1 - y0 };
    double len = vector_length(dir);
    if (len == 0.0) return;

    Vector perp = vector_scale(normal_vector(dir), 1.0 / len);

    for (int t = -thickness / 2; t <= thickness / 2; t++) {
      Vector offset = vector_scale(perp, (double)t);
      draw_line(buffer, img_width, img_height,
                x0 + offset.x, y0 + offset.y,
                x1 + offset.x, y1 + offset.y,
                color, 1);
    }
    return;
  }

  // Wu's algorithm
  int steep = fabs(y1 - y0) > fabs(x1 - x0);

  if (steep) {
    double tmp;
    tmp = x0; x0 = y0; y0 = tmp;
    tmp = x1; x1 = y1; y1 = tmp;
  }

  if (x0 > x1) {
    double tmp;
    tmp = x0; x0 = x1; x1 = tmp;
    tmp = y0; y0 = y1; y1 = tmp;
  }

  double dx = x1 - x0;
  double dy = y1 - y0;
  double gradient = (dx == 0.0) ? 1.0 : dy / dx;

  // First endpoint
  double xend = round(x0);
  double yend = y0 + gradient * (xend - x0);
  double xgap = rfpart(x0 + 0.5);
  int xpxl1 = (int)xend;
  int ypxl1 = (int)floor(yend);

  if (steep) {
    set_pixel_alpha(buffer, img_width, img_height, ypxl1, xpxl1, color, rfpart(yend) * xgap);
    set_pixel_alpha(buffer, img_width, img_height, ypxl1 + 1, xpxl1, color, fpart(yend) * xgap);
  } else {
    set_pixel_alpha(buffer, img_width, img_height, xpxl1, ypxl1, color, rfpart(yend) * xgap);
    set_pixel_alpha(buffer, img_width, img_height, xpxl1, ypxl1 + 1, color, fpart(yend) * xgap);
  }

  double intery = yend + gradient;

  // Second endpoint
  xend = round(x1);
  yend = y1 + gradient * (xend - x1);
  xgap = fpart(x1 + 0.5);
  int xpxl2 = (int)xend;
  int ypxl2 = (int)floor(yend);

  if (steep) {
    set_pixel_alpha(buffer, img_width, img_height, ypxl2, xpxl2, color, rfpart(yend) * xgap);
    set_pixel_alpha(buffer, img_width, img_height, ypxl2 + 1, xpxl2, color, fpart(yend) * xgap);
  } else {
    set_pixel_alpha(buffer, img_width, img_height, xpxl2, ypxl2, color, rfpart(yend) * xgap);
    set_pixel_alpha(buffer, img_width, img_height, xpxl2, ypxl2 + 1, color, fpart(yend) * xgap);
  }

  // Main loop
  if (steep) {
    for (int x = xpxl1 + 1; x < xpxl2; x++) {
      int y = (int)floor(intery);
      set_pixel_alpha(buffer, img_width, img_height, y, x, color, rfpart(intery));
      set_pixel_alpha(buffer, img_width, img_height, y + 1, x, color, fpart(intery));
      intery += gradient;
    }
  } else {
    for (int x = xpxl1 + 1; x < xpxl2; x++) {
      int y = (int)floor(intery);
      set_pixel_alpha(buffer, img_width, img_height, x, y, color, rfpart(intery));
      set_pixel_alpha(buffer, img_width, img_height, x, y + 1, color, fpart(intery));
      intery += gradient;
    }
  }
}

void draw_circle(Color *buffer, int img_width, int img_height, double cx, double cy, double radius, const Color *fill_color, const Color *outline_color, int outline_thickness) {
  if (!buffer) return;

  int padding = outline_thickness + 2;
  int start_x = (int)floor(cx - radius) - padding;
  int end_x   = (int)ceil(cx + radius) + padding;
  int start_y = (int)floor(cy - radius) - padding;
  int end_y   = (int)ceil(cy + radius) + padding;

  if (start_x < 0) start_x = 0;
  if (start_y < 0) start_y = 0;
  if (end_x >= img_width) end_x = img_width - 1;
  if (end_y >= img_height) end_y = img_height - 1;

  Vector center = { cx, cy };
  double half_thickness = (double)outline_thickness / 2.0;

  for (int py = start_y; py <= end_y; py++) {
    for (int px = start_x; px <= end_x; px++) {
      double fill_coverage = 0.0;
      double outline_coverage = 0.0;
      int samples = 4;
      double sample_weight = 1.0 / (double)(samples * samples);

      for (int sy = 0; sy < samples; sy++) {
        for (int sx = 0; sx < samples; sx++) {
          Vector sample = {
            (double)px + ((double)sx + 0.5) / (double)samples,
            (double)py + ((double)sy + 0.5) / (double)samples
          };

          double dist = vector_length(subtract_vectors(sample, center));

          if (dist <= radius) {
            double edge_dist = radius - dist;
            if (outline_color && edge_dist <= (double)outline_thickness) {
              outline_coverage += sample_weight;
            } else if (fill_color) {
              fill_coverage += sample_weight;
            }
          } else if (outline_color && dist <= radius + half_thickness + 1.0) {
            double aa_alpha = 1.0 - (dist - radius - half_thickness);
            if (aa_alpha > 0.0 && aa_alpha <= 1.0) {
              outline_coverage += sample_weight * aa_alpha;
            }
          }
        }
      }

      if (outline_coverage > 0.0 || fill_coverage > 0.0) {
        Color bg = get_pixel(buffer, img_width, img_height, px, py);
        Color result = bg;

        if (fill_color && fill_coverage > 0.0) {
          result = blend_colors(result, *fill_color, fill_coverage);
        }
        if (outline_color && outline_coverage > 0.0) {
          result = blend_colors(result, *outline_color, outline_coverage);
        }

        set_pixel(buffer, img_width, img_height, px, py, result);
      }
    }
  }
}

void draw_rectangle(Color *buffer, int img_width, int img_height, const Rectangle *rect, const Color *fill_color, const Color *outline_color, int outline_thickness) {
  if (!buffer || !rect) return;

  // Find bounding box
  double min_x = rect->corner[0].x, max_x = rect->corner[0].x;
  double min_y = rect->corner[0].y, max_y = rect->corner[0].y;

  for (int i = 1; i < 4; i++) {
    if (rect->corner[i].x < min_x) min_x = rect->corner[i].x;
    if (rect->corner[i].x > max_x) max_x = rect->corner[i].x;
    if (rect->corner[i].y < min_y) min_y = rect->corner[i].y;
    if (rect->corner[i].y > max_y) max_y = rect->corner[i].y;
  }

  int padding = outline_thickness + 2;
  int start_x = (int)floor(min_x) - padding;
  int end_x   = (int)ceil(max_x) + padding;
  int start_y = (int)floor(min_y) - padding;
  int end_y   = (int)ceil(max_y) + padding;

  if (start_x < 0) start_x = 0;
  if (start_y < 0) start_y = 0;
  if (end_x >= img_width) end_x = img_width - 1;
  if (end_y >= img_height) end_y = img_height - 1;

  double half_thickness = (double)outline_thickness / 2.0;

  for (int py = start_y; py <= end_y; py++) {
    for (int px = start_x; px <= end_x; px++) {
      double fill_coverage = 0.0;
      double outline_coverage = 0.0;
      int samples = 4;
      double sample_weight = 1.0 / (double)(samples * samples);

      for (int sy = 0; sy < samples; sy++) {
        for (int sx = 0; sx < samples; sx++) {
          Vector sample = {
            (double)px + ((double)sx + 0.5) / (double)samples,
            (double)py + ((double)sy + 0.5) / (double)samples
          };

          double edge_dist = point_to_rect_edge_distance(rect, sample);

          // Check if sample point is inside rectangle
          int inside = 1;
          for (int i = 0; i < 4; i++) {
            Vector edge = subtract_vectors(rect->corner[(i + 1) % 4], rect->corner[i]);
            Vector to_point = subtract_vectors(sample, rect->corner[i]);
            if (cross_product_2d(edge, to_point) > 0) {
              inside = 0;
              break;
            }
          }

          if (inside) {
            if (outline_color && edge_dist <= (double)outline_thickness) {
              outline_coverage += sample_weight;
            } else if (fill_color) {
              fill_coverage += sample_weight;
            }
          } else if (outline_color && edge_dist <= half_thickness + 1.0) {
            double aa_alpha = 1.0 - (edge_dist - half_thickness);
            if (aa_alpha > 0.0 && aa_alpha <= 1.0) {
              outline_coverage += sample_weight * aa_alpha;
            }
          }
        }
      }

      if (outline_coverage > 0.0 || fill_coverage > 0.0) {
        Color bg = get_pixel(buffer, img_width, img_height, px, py);
        Color result = bg;

        if (fill_color && fill_coverage > 0.0) {
          result = blend_colors(result, *fill_color, fill_coverage);
        }
        if (outline_color && outline_coverage > 0.0) {
          result = blend_colors(result, *outline_color, outline_coverage);
        }

        set_pixel(buffer, img_width, img_height, px, py, result);
      }
    }
  }
}

// ============================================================================
// Text Rendering
// ============================================================================


static void draw_char(Color *buffer, int img_width, int img_height,
                      const int *bitmap, int char_width, int char_height,
                      int start_x, int start_y) {
  for (int row = 0; row < char_height; row++) {
    for (int col = 0; col < char_width; col++) {
      if (bitmap[row] & (1 << (char_width - 1 - col))) {
        set_pixel(buffer, img_width, img_height, start_x + col, start_y + row, COLOR_BLACK);
      }
    }
  }
}

static void draw_text(Color *buffer, int img_width, int img_height,
                      const char *text, int center_x, int center_y, Color color) {
  const int char_width = 5;
  const int char_height = 7;
  const int spacing = 1;
  
  // Simple bitmap font for A-Z, 0-9
  const int font[36][7] = {
    // A-Z
    {0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001}, // A
    {0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b11110}, // B
    {0b01110, 0b10001, 0b10000, 0b10000, 0b10000, 0b10001, 0b01110}, // C
    {0b11110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11110}, // D
    {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111}, // E
    {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000}, // F
    {0b01110, 0b10001, 0b10000, 0b10111, 0b10001, 0b10001, 0b01110}, // G
    {0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001}, // H
    {0b01110, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110}, // I
    {0b00111, 0b00010, 0b00010, 0b00010, 0b00010, 0b10010, 0b01100}, // J
    {0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001}, // K
    {0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111}, // L
    {0b10001, 0b11011, 0b10101, 0b10101, 0b10001, 0b10001, 0b10001}, // M
    {0b10001, 0b11001, 0b10101, 0b10011, 0b10001, 0b10001, 0b10001}, // N
    {0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110}, // O
    {0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000}, // P
    {0b01110, 0b10001, 0b10001, 0b10001, 0b10101, 0b10010, 0b01101}, // Q
    {0b11110, 0b10001, 0b10001, 0b11110, 0b10100, 0b10010, 0b10001}, // R
    {0b01110, 0b10001, 0b10000, 0b01110, 0b00001, 0b10001, 0b01110}, // S
    {0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100}, // T
    {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110}, // U
    {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01010, 0b00100}, // V
    {0b10001, 0b10001, 0b10001, 0b10101, 0b10101, 0b10101, 0b01010}, // W
    {0b10001, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001}, // X
    {0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100}, // Y
    {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111}, // Z
    // 0-9
    {0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110}, // 0
    {0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110}, // 1
    {0b01110, 0b10001, 0b00001, 0b00110, 0b01000, 0b10000, 0b11111}, // 2
    {0b01110, 0b10001, 0b00001, 0b00110, 0b00001, 0b10001, 0b01110}, // 3
    {0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010}, // 4
    {0b11111, 0b10000, 0b11110, 0b00001, 0b00001, 0b10001, 0b01110}, // 5
    {0b00110, 0b01000, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110}, // 6
    {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b01000, 0b01000}, // 7
    {0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110}, // 8
    {0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b00010, 0b01100}  // 9
  };

  int len = strlen(text);
  int total_width = len * char_width + (len - 1) * spacing;
  int start_x = center_x - total_width / 2;
  int start_y = center_y - char_height / 2;

  for (int i = 0; i < len; i++) {
    char c = text[i];
    int font_index = -1;

    if (c >= 'A' && c <= 'Z') {
      font_index = c - 'A';
    } else if (c >= 'a' && c <= 'z') {
      font_index = c - 'a';
    } else if (c >= '0' && c <= '9') {
      font_index = 26 + (c - '0');
    }

    if (font_index >= 0) {
      int x = start_x + i * (char_width + spacing);
      for (int row = 0; row < char_height; row++) {
        for (int col = 0; col < char_width; col++) {
          if (font[font_index][row] & (1 << (char_width - 1 - col))) {
            set_pixel(buffer, img_width, img_height, x + col, start_y + row, color);
          }
        }
      }
    }
  }
}

void draw_space_label(Color *buffer, int img_width, int img_height,
                      const Rectangle *pixel_rect, const char *name) {
  if (!buffer || !pixel_rect || !name) return;

  int max_chars = 4;
  int len = strlen(name);
  if (len > max_chars) return;

  // Find center of rectangle
  double center_x = 0.0;
  double center_y = 0.0;
  for (int i = 0; i < 4; i++) {
    center_x += pixel_rect->corner[i].x;
    center_y += pixel_rect->corner[i].y;
  }
  center_x /= 4.0;
  center_y /= 4.0;

  draw_text(buffer, img_width, img_height, name, (int)center_x, (int)center_y, COLOR_BLACK);
}

void draw_level_label(Color *buffer, int img_width, int img_height, int level, int margin) {
  const int char_width = 5;
  const int char_height = 7;
  const int spacing = 1;
  
  const int letter_L[7] = {
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b11111
  };
  const int letter_e[7] = {
    0b00000,
    0b01110,
    0b10001,
    0b11111,
    0b10000,
    0b10001,
    0b01110
  };
  const int letter_v[7] = {
    0b00000,
    0b00000,
    0b10001,
    0b10001,
    0b10001,
    0b01010,
    0b00100
  };
  const int letter_l[7] = {
    0b01100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b01110
  };
  const int letter_space[7] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000
  };
  const int digits[10][7] = {
    {0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110},
    {0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110},
    {0b01110, 0b10001, 0b00001, 0b00110, 0b01000, 0b10000, 0b11111},
    {0b01110, 0b10001, 0b00001, 0b00110, 0b00001, 0b10001, 0b01110},
    {0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010},
    {0b11111, 0b10000, 0b11110, 0b00001, 0b00001, 0b10001, 0b01110},
    {0b00110, 0b01000, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110},
    {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b01000, 0b01000},
    {0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110},
    {0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b00010, 0b01100}
  };

  int x = margin;
  int y = margin;

  draw_char(buffer, img_width, img_height, letter_L, char_width, char_height, x, y);
  x += char_width + spacing;
  draw_char(buffer, img_width, img_height, letter_e, char_width, char_height, x, y);
  x += char_width + spacing;
  draw_char(buffer, img_width, img_height, letter_v, char_width, char_height, x, y);
  x += char_width + spacing;
  draw_char(buffer, img_width, img_height, letter_e, char_width, char_height, x, y);
  x += char_width + spacing;
  draw_char(buffer, img_width, img_height, letter_l, char_width, char_height, x, y);
  x += char_width + spacing;
  draw_char(buffer, img_width, img_height, letter_space, char_width, char_height, x, y);
  x += char_width + spacing;

  if (level == 0) {
    draw_char(buffer, img_width, img_height, digits[0], char_width, char_height, x, y);
  } else {
    int temp = level;
    int digit_count = 0;
    int level_digits[10];
    
    while (temp > 0) {
      level_digits[digit_count++] = temp % 10;
      temp /= 10;
    }
    
    for (int i = digit_count - 1; i >= 0; i--) {
      draw_char(buffer, img_width, img_height, digits[level_digits[i]], char_width, char_height, x, y);
      x += char_width + spacing;
    }
  }
}

// ============================================================================
// Scale Bar
// ============================================================================

void draw_scale_bar(Color *buffer, int img_width, int img_height, int pixels_per_unit, int margin) {
  int bar_length = pixels_per_unit;
  int bar_height = 6;
  int tick_height = 10;

  int x_start = margin;
  int y_bar = img_height - margin - tick_height;

  // Horizontal bar
  for (int y = y_bar; y < y_bar + bar_height; y++) {
    for (int x = x_start; x < x_start + bar_length; x++) {
      set_pixel(buffer, img_width, img_height, x, y, COLOR_BLACK);
    }
  }

  // Left tick
  for (int y = y_bar - (tick_height - bar_height); y < y_bar + bar_height; y++) {
    for (int x = x_start; x < x_start + 2; x++) {
      set_pixel(buffer, img_width, img_height, x, y, COLOR_BLACK);
    }
  }

  // Right tick
  for (int y = y_bar - (tick_height - bar_height); y < y_bar + bar_height; y++) {
    for (int x = x_start + bar_length - 2; x < x_start + bar_length; x++) {
      set_pixel(buffer, img_width, img_height, x, y, COLOR_BLACK);
    }
  }

  // Draw "1"
  int one_x = x_start + (bar_length / 2);
  int one_y = y_bar - tick_height - 12;
  int one_height = 10;
  int one_width = 6;

  for (int y = one_y; y < one_y + one_height; y++) {
    set_pixel(buffer, img_width, img_height, one_x, y, COLOR_BLACK);
    set_pixel(buffer, img_width, img_height, one_x + 1, y, COLOR_BLACK);
  }

  for (int x = one_x - 2; x < one_x + one_width - 2; x++) {
    set_pixel(buffer, img_width, img_height, x, one_y + one_height - 1, COLOR_BLACK);
    set_pixel(buffer, img_width, img_height, x, one_y + one_height - 2, COLOR_BLACK);
  }

  set_pixel(buffer, img_width, img_height, one_x - 1, one_y + 1, COLOR_BLACK);
  set_pixel(buffer, img_width, img_height, one_x - 2, one_y + 2, COLOR_BLACK);
}

// ============================================================================
// Lot Rendering
// ============================================================================

static void calculate_lot_bounds(const Lot lot, int level, double *min_x, double *min_y, double *max_x, double *max_y) {
  *min_x = *min_y = 1e9;
  *max_x = *max_y = -1e9;

  for (int i = 0; i < lot.space_count; i++) {
    if (lot.spaces[i].location.level == level) {
      Rectangle rect = get_space_rectangle(lot.spaces[i]);
      for (int j = 0; j < 4; j++) {
        if (rect.corner[j].x < *min_x) *min_x = rect.corner[j].x;
        if (rect.corner[j].x > *max_x) *max_x = rect.corner[j].x;
        if (rect.corner[j].y < *min_y) *min_y = rect.corner[j].y;
        if (rect.corner[j].y > *max_y) *max_y = rect.corner[j].y;
      }
    }
  }

  for (int i = 0; i < lot.path_count; i++) {
    if (lot.paths[i].start_point.level == level) {
      Location end = get_endpoint(lot.paths[i]);
      if (lot.paths[i].start_point.x < *min_x) *min_x = lot.paths[i].start_point.x;
      if (lot.paths[i].start_point.x > *max_x) *max_x = lot.paths[i].start_point.x;
      if (lot.paths[i].start_point.y < *min_y) *min_y = lot.paths[i].start_point.y;
      if (lot.paths[i].start_point.y > *max_y) *max_y = lot.paths[i].start_point.y;
      if (end.x < *min_x) *min_x = end.x;
      if (end.x > *max_x) *max_x = end.x;
      if (end.y < *min_y) *min_y = end.y;
      if (end.y > *max_y) *max_y = end.y;
    }
  }

  if (lot.entrance.level == level) {
    if (lot.entrance.x < *min_x) *min_x = lot.entrance.x;
    if (lot.entrance.x > *max_x) *max_x = lot.entrance.x;
    if (lot.entrance.y < *min_y) *min_y = lot.entrance.y;
    if (lot.entrance.y > *max_y) *max_y = lot.entrance.y;
  }

  if (lot.POI.level == level) {
    if (lot.POI.x < *min_x) *min_x = lot.POI.x;
    if (lot.POI.x > *max_x) *max_x = lot.POI.x;
    if (lot.POI.y < *min_y) *min_y = lot.POI.y;
    if (lot.POI.y > *max_y) *max_y = lot.POI.y;
  }

  for (int i = 0; i < lot.up_count; i++) {
    if (lot.ups[i].level == level) {
      if (lot.ups[i].x < *min_x) *min_x = lot.ups[i].x;
      if (lot.ups[i].x > *max_x) *max_x = lot.ups[i].x;
      if (lot.ups[i].y < *min_y) *min_y = lot.ups[i].y;
      if (lot.ups[i].y > *max_y) *max_y = lot.ups[i].y;
    }
  }

  for (int i = 0; i < lot.down_count; i++) {
    if (lot.downs[i].level == level) {
      if (lot.downs[i].x < *min_x) *min_x = lot.downs[i].x;
      if (lot.downs[i].x > *max_x) *max_x = lot.downs[i].x;
      if (lot.downs[i].y < *min_y) *min_y = lot.downs[i].y;
      if (lot.downs[i].y > *max_y) *max_y = lot.downs[i].y;
    }
  }

  *min_x -= 2.0;
  *min_y -= 2.0;
  *max_x += 2.0;
  *max_y += 2.0;
}

static Rectangle world_to_pixel_rect(const Rectangle world_rect, double pixels_per_unit, double min_x, double max_y) {
  Rectangle pixel_rect;
  for (int i = 0; i < 4; i++) {
    pixel_rect.corner[i].x = (world_rect.corner[i].x - min_x) * pixels_per_unit;
    pixel_rect.corner[i].y = (max_y - world_rect.corner[i].y) * pixels_per_unit;
  }
  return pixel_rect;
}

int lot_to_ppm(const Lot lot, const char *filename, int level, int pixels_per_unit) {
  if (!filename || pixels_per_unit <= 0) return -1;

  double min_x, min_y, max_x, max_y;
  calculate_lot_bounds(lot, level, &min_x, &min_y, &max_x, &max_y);

  int img_width = (int)((max_x - min_x) * pixels_per_unit);
  int img_height = (int)((max_y - min_y) * pixels_per_unit);

  if (img_width <= 0 || img_height <= 0) return -1;

  Color *buffer = malloc(img_width * img_height * sizeof(Color));
  if (!buffer) return -1;

  for (int i = 0; i < img_width * img_height; i++) {
    buffer[i] = COLOR_BACKGROUND;
  }

  #define TO_PX_X(wx) (((wx) - min_x) * pixels_per_unit)
  #define TO_PX_Y(wy) ((max_y - (wy)) * pixels_per_unit)

  // Draw paths
  for (int i = 0; i < lot.path_count; i++) {
    if (lot.paths[i].start_point.level == level) {
      Location end = get_endpoint(lot.paths[i]);
      draw_line(
        buffer,
        img_width,
        img_height,
        TO_PX_X(lot.paths[i].start_point.x),
        TO_PX_Y(lot.paths[i].start_point.y),
        TO_PX_X(end.x),
        TO_PX_Y(end.y),
        COLOR_PATH,
        5
      );
    }
  }

  // Draw spaces
  for (int i = 0; i < lot.space_count; i++) {
    if (lot.spaces[i].location.level == level) {
      Rectangle world_rect = get_space_rectangle(lot.spaces[i]);
      Rectangle pixel_rect = world_to_pixel_rect(world_rect, pixels_per_unit, min_x, max_y);
      Color fill = get_space_color(lot.spaces[i].type);
      draw_rectangle(buffer, img_width, img_height, &pixel_rect, &fill, &COLOR_BLACK, 2);
      draw_space_label(buffer, img_width, img_height, &pixel_rect, lot.spaces[i].name);
    }
  }

  // Draw entrance
  if (lot.entrance.level == level) {
    draw_circle(buffer, img_width, img_height,
                TO_PX_X(lot.entrance.x), TO_PX_Y(lot.entrance.y),
                pixels_per_unit * 0.8, &COLOR_ENTRANCE, &COLOR_BLACK, 2);
  }

  // Draw POI
  if (lot.POI.level == level) {
    draw_circle(buffer, img_width, img_height,
                TO_PX_X(lot.POI.x), TO_PX_Y(lot.POI.y),
                pixels_per_unit * 0.6, &COLOR_POI, &COLOR_BLACK, 2);
  }

  // Draw ups
  for (int i = 0; i < lot.up_count; i++) {
    if (lot.ups[i].level == level) {
      draw_circle(buffer, img_width, img_height,
                  TO_PX_X(lot.ups[i].x), TO_PX_Y(lot.ups[i].y),
                  pixels_per_unit * 0.5, &COLOR_UP, &COLOR_BLACK, 2);
    }
  }

  // Draw downs
  for (int i = 0; i < lot.down_count; i++) {
    if (lot.downs[i].level == level) {
      draw_circle(buffer, img_width, img_height,
                  TO_PX_X(lot.downs[i].x), TO_PX_Y(lot.downs[i].y),
                  pixels_per_unit * 0.5, &COLOR_DOWN, &COLOR_BLACK, 2);
    }
  }

  #undef TO_PX_X
  #undef TO_PX_Y

  draw_scale_bar(buffer, img_width, img_height, pixels_per_unit, 15);
  draw_level_label(buffer, img_width, img_height, level, 15);

  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    free(buffer);
    return -1;
  }

  fprintf(fp, "P6\n%d %d\n255\n", img_width, img_height);
  for (int i = 0; i < img_width * img_height; i++) {
    fputc(buffer[i].r, fp);
    fputc(buffer[i].g, fp);
    fputc(buffer[i].b, fp);
  }

  fclose(fp);
  free(buffer);
  return 0;
}

int lot_to_ppm_all_levels(const Lot lot, const char *base_filename, int pixels_per_unit) {
  if (!base_filename) return -1;

  char filename[256];
  for (int level = 0; level < lot.level_count; level++) {
    snprintf(filename, sizeof(filename), "%s_level%d.ppm", base_filename, level);
    if (lot_to_ppm(lot, filename, level, pixels_per_unit) != 0) {
      return -1;
    }
  }
  return 0;
}
