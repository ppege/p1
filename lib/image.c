#include "image.h"
#include "data.h"
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

  Color result;
  result.r = (unsigned char)(bg.r * (1.0 - alpha) + fg.r * alpha);
  result.g = (unsigned char)(bg.g * (1.0 - alpha) + fg.g * alpha);
  result.b = (unsigned char)(bg.b * (1.0 - alpha) + fg.b * alpha);
  return result;
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
    Color blended = blend_colors(bg, color, alpha);
    set_pixel(buffer, img_width, img_height, x, y, blended);
  }
}

// ============================================================================
// Math Helpers
// ============================================================================

static double fpart(double x) {
  return x - floor(x);
}

static double rfpart(double x) {
  return 1.0 - fpart(x);
}

static double cross_product(Vector p0, Vector p1, Vector p2) {
  return (p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x);
}

static double point_to_segment_distance(double px, double py,
                                         double x0, double y0,
                                         double x1, double y1) {
  double dx = x1 - x0;
  double dy = y1 - y0;
  double len_sq = dx * dx + dy * dy;

  if (len_sq == 0.0) {
    double dpx = px - x0;
    double dpy = py - y0;
    return sqrt(dpx * dpx + dpy * dpy);
  }

  double t = ((px - x0) * dx + (py - y0) * dy) / len_sq;
  if (t < 0.0) t = 0.0;
  if (t > 1.0) t = 1.0;

  double proj_x = x0 + t * dx;
  double proj_y = y0 + t * dy;
  double dpx = px - proj_x;
  double dpy = py - proj_y;

  return sqrt(dpx * dpx + dpy * dpy);
}

static double point_to_rect_edge_distance(Vector corners[4], double px, double py) {
  double min_dist = 1e9;

  for (int i = 0; i < 4; i++) {
    int j = (i + 1) % 4;
    double dist = point_to_segment_distance(px, py,
                                             corners[i].x, corners[i].y,
                                             corners[j].x, corners[j].y);
    if (dist < min_dist) {
      min_dist = dist;
    }
  }

  return min_dist;
}

// ============================================================================
// Drawing Functions
// ============================================================================

void draw_line(Color *buffer, int img_width, int img_height,
               double x0, double y0, double x1, double y1,
               Color color, int thickness) {
  // For thickness > 1, draw multiple parallel lines
  if (thickness > 1) {
    double dx = x1 - x0;
    double dy = y1 - y0;
    double len = sqrt(dx * dx + dy * dy);

    if (len == 0.0) return;

    // Perpendicular unit vector
    double px = -dy / len;
    double py = dx / len;

    for (int t = -thickness / 2; t <= thickness / 2; t++) {
      double offset = (double)t;
      draw_line(buffer, img_width, img_height,
                x0 + px * offset, y0 + py * offset,
                x1 + px * offset, y1 + py * offset,
                color, 1);
    }
    return;
  }

  // Wu's algorithm for single-pixel anti-aliased line
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

void draw_circle(Color *buffer, int img_width, int img_height,
                 double cx, double cy, double radius,
                 const Color *fill_color,
                 const Color *outline_color,
                 int outline_thickness) {
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

  double half_thickness = (double)outline_thickness / 2.0;

  for (int py = start_y; py <= end_y; py++) {
    for (int px = start_x; px <= end_x; px++) {
      // Supersampling 4x4
      double fill_coverage = 0.0;
      double outline_coverage = 0.0;
      int samples = 4;
      double sample_weight = 1.0 / (double)(samples * samples);

      for (int sy = 0; sy < samples; sy++) {
        for (int sx = 0; sx < samples; sx++) {
          double sample_x = (double)px + ((double)sx + 0.5) / (double)samples;
          double sample_y = (double)py + ((double)sy + 0.5) / (double)samples;

          double dx = sample_x - cx;
          double dy = sample_y - cy;
          double dist = sqrt(dx * dx + dy * dy);

          if (dist <= radius) {
            // Inside circle
            double edge_dist = radius - dist;
            if (outline_color && edge_dist <= (double)outline_thickness) {
              outline_coverage += sample_weight;
            } else if (fill_color) {
              fill_coverage += sample_weight;
            }
          } else {
            // Outside but check anti-aliased edge
            if (outline_color && dist <= radius + half_thickness + 1.0) {
              double aa_alpha = 1.0 - (dist - radius - half_thickness);
              if (aa_alpha > 0.0 && aa_alpha <= 1.0) {
                outline_coverage += sample_weight * aa_alpha;
              }
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

void draw_rectangle(Color *buffer, int img_width, int img_height,
                    const Rectangle *rect,
                    const Color *fill_color,
                    const Color *outline_color,
                    int outline_thickness) {
  if (!buffer || !rect) return;

  double min_x = rect->corner[0].x;
  double max_x = rect->corner[0].x;
  double min_y = rect->corner[0].y;
  double max_y = rect->corner[0].y;

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

  Vector corners[4];
  for (int i = 0; i < 4; i++) {
    corners[i] = rect->corner[i];
  }

  // Determine winding order by checking sign of cross product for first edge
  // Use center point of rectangle to test
  double center_x = (corners[0].x + corners[1].x + corners[2].x + corners[3].x) / 4.0;
  double center_y = (corners[0].y + corners[1].y + corners[2].y + corners[3].y) / 4.0;
  Vector center = {center_x, center_y};
  double test_cross = cross_product(corners[0], corners[1], center);
  int expect_negative = (test_cross < 0);

  double half_thickness = (double)outline_thickness / 2.0;

  for (int py = start_y; py <= end_y; py++) {
    for (int px = start_x; px <= end_x; px++) {
      double fill_coverage = 0.0;
      double outline_coverage = 0.0;
      int samples = 4;
      double sample_weight = 1.0 / (double)(samples * samples);

      for (int sy = 0; sy < samples; sy++) {
        for (int sx = 0; sx < samples; sx++) {
          double sample_x = (double)px + ((double)sx + 0.5) / (double)samples;
          double sample_y = (double)py + ((double)sy + 0.5) / (double)samples;

          double edge_dist = point_to_rect_edge_distance(corners, sample_x, sample_y);

          int inside = 1;
          for (int i = 0; i < 4; i++) {
            int j = (i + 1) % 4;
            Vector p0 = corners[i];
            Vector p1 = corners[j];
            Vector p = {sample_x, sample_y};
            double cross = cross_product(p0, p1, p);
            // Check against expected sign based on winding order
            if (expect_negative) {
              if (cross > 0) {
                inside = 0;
                break;
              }
            } else {
              if (cross < 0) {
                inside = 0;
                break;
              }
            }
          }

          if (inside) {
            if (outline_color && edge_dist <= (double)outline_thickness) {
              outline_coverage += sample_weight;
            } else if (fill_color) {
              fill_coverage += sample_weight;
            }
          } else {
            if (outline_color && edge_dist <= half_thickness + 1.0) {
              double aa_alpha = 1.0 - (edge_dist - half_thickness);
              if (aa_alpha > 0.0 && aa_alpha <= 1.0) {
                outline_coverage += sample_weight * aa_alpha;
              }
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
// Lot Rendering
// ============================================================================

static void calculate_lot_bounds(const Lot *lot, int level,
                                  double *min_x, double *min_y,
                                  double *max_x, double *max_y) {
  *min_x = *min_y = 1e9;
  *max_x = *max_y = -1e9;

  // Check spaces using get_space_rectangle
  for (int i = 0; i < lot->space_count; i++) {
    if (lot->spaces[i].location.level == level) {
      Rectangle rect = get_space_rectangle(&lot->spaces[i]);
      for (int j = 0; j < 4; j++) {
        if (rect.corner[j].x < *min_x) *min_x = rect.corner[j].x;
        if (rect.corner[j].x > *max_x) *max_x = rect.corner[j].x;
        if (rect.corner[j].y < *min_y) *min_y = rect.corner[j].y;
        if (rect.corner[j].y > *max_y) *max_y = rect.corner[j].y;
      }
    }
  }

  // Check paths
  for (int i = 0; i < lot->path_count; i++) {
    if (lot->paths[i].start_point.level == level) {
      double x1 = lot->paths[i].start_point.x;
      double y1 = lot->paths[i].start_point.y;
      double x2 = x1 + lot->paths[i].vector.x;
      double y2 = y1 + lot->paths[i].vector.y;

      if (x1 < *min_x) *min_x = x1;
      if (x2 < *min_x) *min_x = x2;
      if (x1 > *max_x) *max_x = x1;
      if (x2 > *max_x) *max_x = x2;
      if (y1 < *min_y) *min_y = y1;
      if (y2 < *min_y) *min_y = y2;
      if (y1 > *max_y) *max_y = y1;
      if (y2 > *max_y) *max_y = y2;
    }
  }

  // Check entrance
  if (lot->entrance.level == level) {
    if (lot->entrance.x < *min_x) *min_x = lot->entrance.x;
    if (lot->entrance.x > *max_x) *max_x = lot->entrance.x;
    if (lot->entrance.y < *min_y) *min_y = lot->entrance.y;
    if (lot->entrance.y > *max_y) *max_y = lot->entrance.y;
  }

  // Check POI
  if (lot->POI.level == level) {
    if (lot->POI.x < *min_x) *min_x = lot->POI.x;
    if (lot->POI.x > *max_x) *max_x = lot->POI.x;
    if (lot->POI.y < *min_y) *min_y = lot->POI.y;
    if (lot->POI.y > *max_y) *max_y = lot->POI.y;
  }

  // Check ups
  for (int i = 0; i < lot->up_count; i++) {
    if (lot->ups[i].level == level) {
      if (lot->ups[i].x < *min_x) *min_x = lot->ups[i].x;
      if (lot->ups[i].x > *max_x) *max_x = lot->ups[i].x;
      if (lot->ups[i].y < *min_y) *min_y = lot->ups[i].y;
      if (lot->ups[i].y > *max_y) *max_y = lot->ups[i].y;
    }
  }

  // Check downs
  for (int i = 0; i < lot->down_count; i++) {
    if (lot->downs[i].level == level) {
      if (lot->downs[i].x < *min_x) *min_x = lot->downs[i].x;
      if (lot->downs[i].x > *max_x) *max_x = lot->downs[i].x;
      if (lot->downs[i].y < *min_y) *min_y = lot->downs[i].y;
      if (lot->downs[i].y > *max_y) *max_y = lot->downs[i].y;
    }
  }

  // Add padding
  double padding = 2.0;
  *min_x -= padding;
  *min_y -= padding;
  *max_x += padding;
  *max_y += padding;
}

// Convert world-space Rectangle to pixel-space Rectangle
static Rectangle world_to_pixel_rect(const Rectangle *world_rect,
                                      double pixels_per_unit,
                                      double min_x, double max_y) {
  Rectangle pixel_rect;
  for (int i = 0; i < 4; i++) {
    pixel_rect.corner[i].x = (world_rect->corner[i].x - min_x) * pixels_per_unit;
    pixel_rect.corner[i].y = (max_y - world_rect->corner[i].y) * pixels_per_unit;
  }
  return pixel_rect;
}

// ============================================================================
// Scale Bar
// ============================================================================

void draw_scale_bar(Color *buffer, int img_width, int img_height,
                    int pixels_per_unit, int margin) {
  int bar_length = pixels_per_unit;
  int bar_height = 6;
  int tick_height = 10;

  // Position in bottom-left corner
  int x_start = margin;
  int y_bar = img_height - margin - tick_height;

  // Draw horizontal bar
  for (int y = y_bar; y < y_bar + bar_height; y++) {
    for (int x = x_start; x < x_start + bar_length; x++) {
      set_pixel(buffer, img_width, img_height, x, y, COLOR_BLACK);
    }
  }

  // Draw left tick
  for (int y = y_bar - (tick_height - bar_height); y < y_bar + bar_height; y++) {
    for (int x = x_start; x < x_start + 2; x++) {
      set_pixel(buffer, img_width, img_height, x, y, COLOR_BLACK);
    }
  }

  // Draw right tick
  for (int y = y_bar - (tick_height - bar_height); y < y_bar + bar_height; y++) {
    for (int x = x_start + bar_length - 2; x < x_start + bar_length; x++) {
      set_pixel(buffer, img_width, img_height, x, y, COLOR_BLACK);
    }
  }

  // Draw "1" as a simple vertical line with a base
  int one_x = x_start + (bar_length / 2);
  int one_y = y_bar - tick_height - 12;
  int one_height = 10;
  int one_width = 6;

  // Vertical stroke
  for (int y = one_y; y < one_y + one_height; y++) {
    set_pixel(buffer, img_width, img_height, one_x, y, COLOR_BLACK);
    set_pixel(buffer, img_width, img_height, one_x + 1, y, COLOR_BLACK);
  }

  // Bottom base
  for (int x = one_x - 2; x < one_x + one_width - 2; x++) {
    set_pixel(buffer, img_width, img_height, x, one_y + one_height - 1, COLOR_BLACK);
    set_pixel(buffer, img_width, img_height, x, one_y + one_height - 2, COLOR_BLACK);
  }

  // Top serif
  set_pixel(buffer, img_width, img_height, one_x - 1, one_y + 1, COLOR_BLACK);
  set_pixel(buffer, img_width, img_height, one_x - 2, one_y + 2, COLOR_BLACK);
}

int lot_to_ppm(const Lot *lot, const char *filename, int level, int pixels_per_unit) {
  if (!lot || !filename || pixels_per_unit <= 0) {
    return -1;
  }

  // Calculate bounds
  double min_x, min_y, max_x, max_y;
  calculate_lot_bounds(lot, level, &min_x, &min_y, &max_x, &max_y);

  // Calculate image dimensions
  int img_width = (int)((max_x - min_x) * pixels_per_unit);
  int img_height = (int)((max_y - min_y) * pixels_per_unit);

  if (img_width <= 0 || img_height <= 0) {
    return -1;
  }

  // Allocate image buffer
  Color *buffer = malloc(img_width * img_height * sizeof(Color));
  if (!buffer) {
    return -1;
  }

  // Fill with background color
  for (int i = 0; i < img_width * img_height; i++) {
    buffer[i] = COLOR_BACKGROUND;
  }

  // Coordinate conversion macros
  #define WORLD_TO_PIXEL_X(wx) (((wx) - min_x) * pixels_per_unit)
  #define WORLD_TO_PIXEL_Y(wy) ((max_y - (wy)) * pixels_per_unit)

  // Draw paths
  for (int i = 0; i < lot->path_count; i++) {
    if (lot->paths[i].start_point.level == level) {
      double x0 = WORLD_TO_PIXEL_X(lot->paths[i].start_point.x);
      double y0 = WORLD_TO_PIXEL_Y(lot->paths[i].start_point.y);
      double x1 = WORLD_TO_PIXEL_X(lot->paths[i].start_point.x + lot->paths[i].vector.x);
      double y1 = WORLD_TO_PIXEL_Y(lot->paths[i].start_point.y + lot->paths[i].vector.y);
      draw_line(buffer, img_width, img_height, x0, y0, x1, y1, COLOR_PATH, 5);
    }
  }

  // Draw spaces using get_space_rectangle
  for (int i = 0; i < lot->space_count; i++) {
    if (lot->spaces[i].location.level == level) {
      Rectangle world_rect = get_space_rectangle(&lot->spaces[i]);
      Rectangle pixel_rect = world_to_pixel_rect(&world_rect, pixels_per_unit, min_x, max_y);
      Color fill = get_space_color(lot->spaces[i].type);
      draw_rectangle(buffer, img_width, img_height, &pixel_rect, &fill, &COLOR_BLACK, 2);
    }
  }

  // Draw entrance
  if (lot->entrance.level == level) {
    double cx = WORLD_TO_PIXEL_X(lot->entrance.x);
    double cy = WORLD_TO_PIXEL_Y(lot->entrance.y);
    draw_circle(buffer, img_width, img_height, cx, cy, pixels_per_unit * 0.8,
                &COLOR_ENTRANCE, &COLOR_BLACK, 2);
  }

  // Draw POI
  if (lot->POI.level == level) {
    double cx = WORLD_TO_PIXEL_X(lot->POI.x);
    double cy = WORLD_TO_PIXEL_Y(lot->POI.y);
    draw_circle(buffer, img_width, img_height, cx, cy, pixels_per_unit * 0.6,
                &COLOR_POI, &COLOR_BLACK, 2);
  }

  // Draw ups
  for (int i = 0; i < lot->up_count; i++) {
    if (lot->ups[i].level == level) {
      double cx = WORLD_TO_PIXEL_X(lot->ups[i].x);
      double cy = WORLD_TO_PIXEL_Y(lot->ups[i].y);
      draw_circle(buffer, img_width, img_height, cx, cy, pixels_per_unit * 0.5,
                  &COLOR_UP, &COLOR_BLACK, 2);
    }
  }

  // Draw downs
  for (int i = 0; i < lot->down_count; i++) {
    if (lot->downs[i].level == level) {
      double cx = WORLD_TO_PIXEL_X(lot->downs[i].x);
      double cy = WORLD_TO_PIXEL_Y(lot->downs[i].y);
      draw_circle(buffer, img_width, img_height, cx, cy, pixels_per_unit * 0.5,
                  &COLOR_DOWN, &COLOR_BLACK, 2);
    }
  }

  #undef WORLD_TO_PIXEL_X
  #undef WORLD_TO_PIXEL_Y

  // Draw scale bar in bottom-left corner
  draw_scale_bar(buffer, img_width, img_height, pixels_per_unit, 15);

  // Write PPM file
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

int lot_to_ppm_all_levels(const Lot *lot, const char *base_filename, int pixels_per_unit) {
  if (!lot || !base_filename) {
    return -1;
  }

  char filename[256];
  for (int level = 0; level < lot->level_count; level++) {
    snprintf(filename, sizeof(filename), "%s_level%d.ppm", base_filename, level);
    if (lot_to_ppm(lot, filename, level, pixels_per_unit) != 0) {
      return -1;
    }
  }
  return 0;
}
