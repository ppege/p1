#pragma once
#include <data.h>

// Color structure for RGB pixels
typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} Color;

// Predefined colors for different elements
static const Color COLOR_BACKGROUND = {255, 255, 255};  // White
static const Color COLOR_STANDARD   = {100, 100, 255};  // Blue
static const Color COLOR_HANDICAP   = {100, 255, 100};  // Green
static const Color COLOR_COMPACT    = {255, 255, 100};  // Yellow
static const Color COLOR_EV         = {100, 255, 255};  // Cyan
static const Color COLOR_PATH       = {150, 150, 150};  // Gray
static const Color COLOR_ENTRANCE   = {255, 100, 100};  // Red
static const Color COLOR_POI        = {255, 100, 255};  // Magenta
static const Color COLOR_UP         = {255, 165, 0};    // Orange
static const Color COLOR_DOWN       = {139, 69, 19};    // Brown
static const Color COLOR_BLACK      = {0, 0, 0};        // Black

void draw_level_label(Color *buffer, int img_width, int img_height, int level, int margin);
void draw_space_label(Color *buffer, int img_width, int img_height, const Rectangle *pixel_rect, const char *name);

/**
 * Draw a rectangle with outline and fill using Wu's anti-aliasing algorithm. 
 */
void draw_rectangle(Color *buffer, int img_width, int img_height,
                    const Rectangle *rect,
                    const Color *fill_color,
                    const Color *outline_color,
                    int outline_thickness);

/**
 * Draw an anti-aliased circle. 
 */
void draw_circle(Color *buffer, int img_width, int img_height,
                 double cx, double cy, double radius,
                 const Color *fill_color,
                 const Color *outline_color,
                 int outline_thickness);

/**
 * Draw an anti-aliased line.
 */
void draw_line(Color *buffer, int img_width, int img_height,
               double x0, double y0, double x1, double y1,
               Color color, int thickness);

/**
 * Draw a scale bar in the corner of the image. 
 */
void draw_scale_bar(Color *buffer, int img_width, int img_height,
                    int pixels_per_unit, int margin);

/**
 * Write a Lot to a PPM file for a specific level.
 */
int lot_to_ppm(const Lot lot, const char *filename, int level, int pixels_per_unit);

/**
 * Write all levels of a Lot to separate PPM files. 
 */
int lot_to_ppm_all_levels(const Lot lot, const char *base_filename, int pixels_per_unit);
