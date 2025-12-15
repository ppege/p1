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

// Helper function to get color based on space type
static Color get_space_color(SpaceType type) {
  switch (type) {
    case Standard: return COLOR_STANDARD;
    case Handicap: return COLOR_HANDICAP;
    case Compact:  return COLOR_COMPACT;
    case EV:       return COLOR_EV;
    default:       return COLOR_STANDARD;
  }
}

// Helper function to read the color of a pixel based on coordinates
static Color get_pixel(Color *buffer, int img_width, int img_height, int x, int y) {
  if (x >= 0 && x < img_width && y >= 0 && y < img_height) {
    return buffer[y * img_width + x];
  }
  return COLOR_BACKGROUND;
}

// Helper function to blend two colors with alpha
static Color blend_colors(Color bg, Color fg, double alpha) {
  // Clamp alpha to [0, 1]
  if (alpha < 0.0) alpha = 0.0;
  if (alpha > 1.0) alpha = 1.0;

  // Each color channel is 100-alpha% background and alpha% foreground if you get what I mean
  // Like, with 1.0 alpha it's 100% foreground, with 0.0 it's 100% background
  // 0.5 alpha gives 0.5 background and 0.5 foreground
  // 0.75 alpha gives 0.25 background and 0.75 foreground, etc; totals to 1.0
  return (Color){
    .r = (unsigned char)(bg.r * (1.0 - alpha) + fg.r * alpha),
    .g = (unsigned char)(bg.g * (1.0 - alpha) + fg.g * alpha),
    .b = (unsigned char)(bg.b * (1.0 - alpha) + fg.b * alpha)
  };
}

// Helper function to set the color of a pixel based on coordinates
static void set_pixel(Color *buffer, int img_width, int img_height, int x, int y, Color color) {
  if (x >= 0 && x < img_width && y >= 0 && y < img_height) {
    buffer[y * img_width + x] = color;
  }
}

// Helper function that combines get_pixel, blend_colors, and set_pixel
// blends a color with the background and sets the pixel
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

// used to find out if we're inside a rectangle
static double point_to_rect_edge_distance(const Rectangle rect, Vector point) {
  double min_dist = 1e9;

  for (int i = 0; i < 4; i++) {
    int j = (i + 1) % 4;
    double dist = point_to_segment_distance(point, rect.corner[i], rect.corner[j]);
    if (dist < min_dist) {
      min_dist = dist;
    }
  }
  return min_dist;
}

// ============================================================================
// Wu's Algorithm Helpers https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm
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

// Wu's line algorithm with thickness support
// Please read https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm to understand this
// Or at least to see that this is a C implementation of the pseudocode under the section "Floating Point Implementation"
void draw_line(Color *buffer, int img_width, int img_height,
               double x0, double y0, double x1, double y1,
               Color color, int thickness) {
  // thickness support outside the scope of Wu's algorithm
  // simply calls itself multiple times offset by perpendicular vectors
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

  // In the wise words of Xiaolin Wu:
  int steep = fabs(y1 - y0) > fabs(x1 - x0); // y difference greater than x difference, the line forms an acute angle with the y-axis
                                             // this is relevant because the algorithm only takes into account the single pixels above and below the line
                                             // you can imagine that as the line gets steeper, this becomes increasingly useless
                                             // so we need to know whether or not the line is "steep" for later

  if (steep) {
    // swap x0 and y0
    double tmp;
    tmp = x0;
    x0 = y0;
    y0 = tmp;

    // swap x1 and y1
    tmp = x1;
    x1 = y1;
    y1 = tmp;

    // this effectively mirrors the line across the y=x axis
    // this lets the algorithm operate in terms of x as the major axis
  }

  // Wu's algorithm assumes left to right drawing, so if (x1, y1) is to the left of (x0, y0), we swap the points
  if (x0 > x1) {
    // swap x0 and x1
    double tmp;
    tmp = x0;
    x0 = x1;
    x1 = tmp;

    // swap y0 and y1
    tmp = y0;
    y0 = y1;
    y1 = tmp;
  }

  // Compute the slope
  double dx = x1 - x0;
  double dy = y1 - y0;
  double gradient = (dx == 0.0) ? 1.0 : dy / dx; // avoid division by zero; setting the gradient to 1.0 doesn't matter
                                                 // the only case where dx == 0 is the degenerate case of a line with no length
                                                 // kind of hard to visualize, but consider this:
                                                 //   1. if the line is vertical (x0 == x1), then steep is true and
                                                 //      x and y are swapped, therefore not necessarily making dx zero
                                                 //   2. if the line is horizontal (y0 == y1), there is no swap so still no dx == 0
                                                 //   3. if the line is a single point (x0 == x1 and y0 == y1), there is no swap,
                                                 //      but dx == 0
                                                 // at this point the main loop doesn't do anything anyway because xpxl1 == xpxl2
                                                 // also, gradient will always be a value between 0 and 1 since dy <= dx

  // The two endpoints of a line are treated specially.
  // First endpoint
  double xend = round(x0); // xend is the first integer x-coordinate where the line begins
  double yend = y0 + gradient * (xend - x0); // corresponding y value for xend
  double xgap = rfpart(x0 + 0.5); // the coverage of the first pixel
  int xpxl1 = (int)xend;
  int ypxl1 = (int)floor(yend); // flooring gives the pixel y below the intersection

  // One pixel receives intensity proportional to the distance
  // from the intersection to its center (rfpart), the other receives the
  // complement (fpart). This is seen in the final argument to set_pixel_alpha below.
  if (steep) {
    // if steep, we swapped x and y earlier, so we need to swap them back here
    set_pixel_alpha(buffer, img_width, img_height, ypxl1, xpxl1, color, rfpart(yend) * xgap);
    set_pixel_alpha(buffer, img_width, img_height, ypxl1 + 1, xpxl1, color, fpart(yend) * xgap);
  } else {
    // otherwise just set the pixel normally
    set_pixel_alpha(buffer, img_width, img_height, xpxl1, ypxl1, color, rfpart(yend) * xgap);
    set_pixel_alpha(buffer, img_width, img_height, xpxl1, ypxl1 + 1, color, fpart(yend) * xgap);
  }

  // intery is the y-value at the next integer x step: yend + slope
  // this serves as the running y coordinate as we iterate through x
  double intery = yend + gradient;

  // Second endpoint; this is similar to the first endpoint except we use fpart instead of rfpart when finding xgap
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

  // Main loop where we iterate over the x values between the two endpoints
  // At each column:
  //  - intery holds the real-valued intersection of the mathematical line.
  //  - floor(intery) gives the pixel just below the line.
  //  - rfpart(intery) gives the intensity weight for the lower pixel.
  //  - fpart(intery) gives the intensity weight for the pixel above it.
  // At each step, we increment intery by the slope (gradient).
  // Again, the only difference between steep and not steep is whether we swap x and y when setting pixels.
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

// Function to draw a filled circle with optional outline.
// The algorithm works by iterating over a bounding box around the circle and
// estimating pixel coverage by sampling multiple sub-pixel locations.
// - Each pixel is subdivided into a small grid (samples × samples).
// - For each sub-sample, we test whether it lies inside the circle, inside the outline band, or outside entirely.
// - Coverage is accumulated as alpha and blended against the buffer.
// Very brute-force technique.
void draw_circle(Color *buffer, int img_width, int img_height, double cx, double cy, double radius, const Color *fill_color, const Color *outline_color, int outline_thickness) {
  if (!buffer) return;

  // First we find the bounding box of the circle plus outline
  int padding = outline_thickness + 2;
  int start_x = (int)floor(cx - radius) - padding;
  int end_x   = (int)ceil(cx + radius) + padding;
  int start_y = (int)floor(cy - radius) - padding;
  int end_y   = (int)ceil(cy + radius) + padding;

  // Clamp to image bounds
  if (start_x < 0) start_x = 0;
  if (start_y < 0) start_y = 0;
  if (end_x >= img_width) end_x = img_width - 1;
  if (end_y >= img_height) end_y = img_height - 1;

  Vector center = { cx, cy };

  // We use half thickness so the outline is centered on the circle edge
  double half_thickness = (double)outline_thickness / 2.0;

  // Main loop
  // Iterate over each pixel in the bounding box
  // For each pixel, estimate how much of it is covered by the fill and outline, respectively.
  for (int py = start_y; py <= end_y; py++) {
    for (int px = start_x; px <= end_x; px++) {

      // Accumulated coverage for the pixel
      double fill_coverage = 0.0;
      double outline_coverage = 0.0;

      // Supersampling parameters; the pixel is subdivided into a 4x4 grid of points
      int samples = 4;
      // Each sample contributes equally to the total coverage
      double sample_weight = 1.0 / (double)(samples * samples);

      // Supersampling loop
      // Each iteration evaluates a single sub-pixel sample
      for (int sy = 0; sy < samples; sy++) {
        for (int sx = 0; sx < samples; sx++) {
          Vector sample = {
            (double)px + ((double)sx + 0.5) / (double)samples,
            (double)py + ((double)sy + 0.5) / (double)samples
          }; // place the sample in the center of the sub-pixel grid cell

          double dist = vector_length(subtract_vectors(sample, center));

          // Determine if the sample is inside the fill area, outline area, or outside
          if (dist <= radius) {
            // We are somewhat inside the circle
            double edge_dist = radius - dist; // distance from the edge of the circle to the sample point

            // Decide if this sample contributes to outline or fill
            if (outline_color && edge_dist <= (double)outline_thickness) {
              // Inside the outline band
              outline_coverage += sample_weight;
            } else if (fill_color) {
              // Inside the fill area
              fill_coverage += sample_weight;
            }
          } else if (outline_color && dist <= radius + half_thickness + 1.0) {
            // We are not inside the circle, but close enough to it to be in the outline's anti-aliasing region
            double aa_alpha = 1.0 - (dist - radius - half_thickness); // alpha falls off linearly with distance
            if (aa_alpha > 0.0 && aa_alpha <= 1.0) {
              // alpha is valid, so we contribute to outline coverage
              outline_coverage += sample_weight * aa_alpha;
            }
          }
        }
      }

      // final pixel composition; first, did we cover anything at all?
      if (outline_coverage > 0.0 || fill_coverage > 0.0) {
        // we did! so, get the background color and assign it a result which we will modify
        Color bg = get_pixel(buffer, img_width, img_height, px, py);
        Color result = bg;

        // if there's some fill coverage, blend it in proportionally to the coverage
        if (fill_color && fill_coverage > 0.0) {
          result = blend_colors(result, *fill_color, fill_coverage);
        }
        // same for outline; the idea is that both can contribute to the final pixel color
        // outline goes last so it appears "on top" of the fill
        if (outline_color && outline_coverage > 0.0) {
          result = blend_colors(result, *outline_color, outline_coverage);
        }

        // finally, set the pixel to the computed result
        set_pixel(buffer, img_width, img_height, px, py, result);
      }
    }
  }
}

// Function to draw a filled rectangle with optional outline.
// works similarly to draw_circle with supersampling for anti-aliasing
void draw_rectangle(Color *buffer, int img_width, int img_height, const Rectangle rect, const Color *fill_color, const Color *outline_color, int outline_thickness) {
  if (!buffer) return;

  // Find bounding box
  double min_x = rect.corner[0].x, max_x = rect.corner[0].x;
  double min_y = rect.corner[0].y, max_y = rect.corner[0].y;

  for (int i = 1; i < 4; i++) {
    if (rect.corner[i].x < min_x) min_x = rect.corner[i].x;
    if (rect.corner[i].x > max_x) max_x = rect.corner[i].x;
    if (rect.corner[i].y < min_y) min_y = rect.corner[i].y;
    if (rect.corner[i].y > max_y) max_y = rect.corner[i].y;
  }

  // padding to ensure outline and anti-aliasing falloff are included
  int padding = outline_thickness + 2;
  int start_x = (int)floor(min_x) - padding;
  int end_x   = (int)ceil(max_x) + padding;
  int start_y = (int)floor(min_y) - padding;
  int end_y   = (int)ceil(max_y) + padding;

  // Clamp to image bounds
  if (start_x < 0) start_x = 0;
  if (start_y < 0) start_y = 0;
  if (end_x >= img_width) end_x = img_width - 1;
  if (end_y >= img_height) end_y = img_height - 1;

  // Outline thickness is centered on the geometric edge
  double half_thickness = (double)outline_thickness / 2.0;

  // Main loop
  // Iterate over each pixel in the bounding box
  // For each pixel, estimate how much of it is covered by the fill and outline, respectively.
  for (int py = start_y; py <= end_y; py++) {
    for (int px = start_x; px <= end_x; px++) {
      // completely analog to draw_circle.
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

          // minimum distance from sample point to rectangle edge
          double edge_dist = point_to_rect_edge_distance(rect, sample);

          // Check if sample point is inside rectangle
          // Uses this rule: for any rectangle with consistently ordered vertices,
          // a point lies inside if it is always on the same side of every edge.
          // cross(edge, to_point) > 0 indicates the point lies outside
          int inside = 1;
          for (int i = 0; i < 4; i++) {
            Vector edge = subtract_vectors(rect.corner[(i + 1) % 4], rect.corner[i]);
            Vector to_point = subtract_vectors(sample, rect.corner[i]);
            if (cross_product_2d(edge, to_point) > 0) {
              inside = 0;
              break;
            }
          }

          if (inside) {
            if (outline_color && edge_dist <= (double)outline_thickness) {
              // sample is inside outline band
              outline_coverage += sample_weight;
            } else if (fill_color) {
              // sample is inside fill area
              fill_coverage += sample_weight;
            }
          } else if (outline_color && edge_dist <= half_thickness + 1.0) {
            // sample is outside rectangle, but within outline anti-aliasing region
            // just like draw_circle
            double aa_alpha = 1.0 - (edge_dist - half_thickness);
            if (aa_alpha > 0.0 && aa_alpha <= 1.0) {
              outline_coverage += sample_weight * aa_alpha;
            }
          }
        }
      }

      // final pixel composition; again, just like draw_circle.
      if (outline_coverage > 0.0 || fill_coverage > 0.0) {
        Color bg = get_pixel(buffer, img_width, img_height, px, py);
        Color result = bg;

        if (fill_color && fill_coverage > 0.0) {
          result = blend_colors(result, *fill_color, fill_coverage);
        }
        if (outline_color && outline_coverage > 0.0) {
          result = blend_colors(result, *outline_color, outline_coverage);
        }

        // set the pixel to the computed result
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

static void draw_text(Color *buffer, int img_width, int img_height, const char *text, int center_x, int center_y, Color color) {
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

  // aligning the text
  int len = strlen(text);
  int total_width = len * char_width + (len - 1) * spacing; // in pixels
  int start_x = center_x - total_width / 2;
  int start_y = center_y - char_height / 2;

  for (int i = 0; i < len; i++) {
    char c = text[i];
    int font_index = -1;

    // ascii math to find index in font array.
    // the character A has ascii value 65 and it continues sequentially until Z (=90).
    if (c >= 'A' && c <= 'Z') {
      // if for example c = 'F', the int for this is 70, so minus A (65)
      // results in 5 which is the index of F in the font array.
      font_index = c - 'A';
    } else if (c >= 'a' && c <= 'z') {
      // same as above but for lowercase letters
      font_index = c - 'a';
    } else if (c >= '0' && c <= '9') {
      // numbers start at index 26 in the font array.
      // so we again normalize to the literal integers 0-9 by subtracting '0' (ascii 48),
      // then add 26 to get the correct index.
      font_index = 26 + (c - '0');
    }

    if (font_index >= 0) { // -1 if invalid
      int x = start_x + i * (char_width + spacing); // self explanatory
      for (int row = 0; row < char_height; row++) {
        for (int col = 0; col < char_width; col++) {
          // for every pixel in the character bitmap, set pixel if bit is 1.
          // & is the bitwise AND operator; << is the left shift operator.
          // char_width - 1 - col gives the correct bit to check for the current column.
          // example: if col is 3, then 1 << (5 - 1 - 3) = 0b00010.
          // this is because "1 << n" shifts this 1 to the left n times.
          // we bitwise AND this with the row, for example 0b01110 (which is the first row of A).
          // you can see vertically that the 4th bit is 1 in both rows,
          // the bitwise AND will yield non-zero, passing the if check and drawing the pixel!
          if (font[font_index][row] & (1 << (char_width - 1 - col))) {
            set_pixel(buffer, img_width, img_height, x + col, start_y + row, color);
          }
        }
      }
    }
  }
}

void draw_space_label(Color *buffer, int img_width, int img_height, const Rectangle pixel_rect, const char *name) {
  if (!buffer || !name) return;

  int max_chars = 10;
  int len = strlen(name);
  if (len > max_chars) return;

  // Find center of rectangle
  double center_x = 0.0;
  double center_y = 0.0;
  for (int i = 0; i < 4; i++) {
    center_x += pixel_rect.corner[i].x;
    center_y += pixel_rect.corner[i].y;
  }
  center_x /= 4.0;
  center_y /= 4.0;

  draw_text(buffer, img_width, img_height, name, (int)center_x, (int)center_y, COLOR_BLACK);
}

void draw_level_label(Color *buffer, int img_width, int img_height, int level, int margin) {
  int x = margin;
  int y = margin;
  char level_text[10];
  sprintf(level_text, "level %d", level);

  draw_text(buffer, img_width, img_height, level_text, x + (int)(2.5 * strlen(level_text)), y, COLOR_BLACK);
}

// ============================================================================
// Scale Bar
// ============================================================================

void draw_scale_bar(Color *buffer, int img_width, int img_height, int pixels_per_unit, int margin) {
  int bar_length = pixels_per_unit;
  int bar_height = 4;
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

int lot_to_ppm(const Lot lot, const char *filename, int level, int pixels_per_unit, Path* nav, int nav_count) {
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

  for (int i = 0; i < img_width; i++) {
    set_pixel(buffer, img_width, img_height, i, 0, COLOR_BLACK);
    set_pixel(buffer, img_width, img_height, i, img_height - 1, COLOR_BLACK);
  }
  for (int i = 0; i < img_height; i++) {
    set_pixel(buffer, img_width, img_height, 0, i, COLOR_BLACK);
    set_pixel(buffer, img_width, img_height, img_width - 1, i, COLOR_BLACK);
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
        pixels_per_unit * 3
      );
    }
  }

  // if nav data is provided, draw it
  for (int i = 0; i < nav_count; i++) {
    if (nav[i].start_point.level == level) {
      Location end = get_endpoint(nav[i]);
      draw_line(
        buffer,
        img_width,
        img_height,
        TO_PX_X(nav[i].start_point.x),
        TO_PX_Y(nav[i].start_point.y),
        TO_PX_X(end.x),
        TO_PX_Y(end.y),
        COLOR_RED,
        pixels_per_unit / 3
      );
    }
  }

  // Draw spaces
  for (int i = 0; i < lot.space_count; i++) {
    if (lot.spaces[i].location.level == level) {
      Rectangle world_rect = get_space_rectangle(lot.spaces[i]);
      Rectangle pixel_rect = world_to_pixel_rect(world_rect, pixels_per_unit, min_x, max_y);
      Color fill = get_space_color(lot.spaces[i].type);
      draw_rectangle(buffer, img_width, img_height, pixel_rect, &fill, &COLOR_BLACK, 2);
      draw_space_label(buffer, img_width, img_height, pixel_rect, lot.spaces[i].name);
    }
  }

  // Draw entrance
  if (lot.entrance.level == level) {
    draw_circle(buffer, img_width, img_height,
                TO_PX_X(lot.entrance.x), TO_PX_Y(lot.entrance.y),
                pixels_per_unit * 0.8, &COLOR_ENTRANCE, &COLOR_BLACK, 0);
  }

  // Draw POI
  if (lot.POI.level == level) {
    draw_circle(buffer, img_width, img_height,
                TO_PX_X(lot.POI.x), TO_PX_Y(lot.POI.y),
                pixels_per_unit * 0.6, &COLOR_POI, &COLOR_BLACK, 0);
  }

  // Draw ups
  for (int i = 0; i < lot.up_count; i++) {
    if (lot.ups[i].level == level) {
      draw_circle(buffer, img_width, img_height,
                  TO_PX_X(lot.ups[i].x), TO_PX_Y(lot.ups[i].y),
                  pixels_per_unit * 0.5, &COLOR_UP, &COLOR_BLACK, 0);
    }
  }

  // Draw downs
  for (int i = 0; i < lot.down_count; i++) {
    if (lot.downs[i].level == level) {
      draw_circle(buffer, img_width, img_height,
                  TO_PX_X(lot.downs[i].x), TO_PX_Y(lot.downs[i].y),
                  pixels_per_unit * 0.5, &COLOR_DOWN, &COLOR_BLACK, 0);
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

int lot_to_ppm_all_levels(const Lot lot, const char *base_filename, int pixels_per_unit, Path* nav, int nav_count) {
  if (!base_filename) return -1;

  char filename[256];
  for (int level = 0; level < lot.level_count; level++) {
    snprintf(filename, sizeof(filename), "%s_level%d.ppm", base_filename, level);
    if (lot_to_ppm(lot, filename, level, pixels_per_unit, nav, nav_count) != 0) {
      return -1;
    }
  }
  return 0;
}
