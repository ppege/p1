#include <math.h>
#include <float.h>
#include "calculations.h"
#include "data.h"

// === Vector operations useful to handle space data ===

Vector subtract_vectors(Vector a, Vector b) {
  return (Vector){ a.x - b.x, a.y - b.y };
};

double vector_dot_product(Vector a, Vector b) {
  return a.x * b.x + a.y * b.y;
};

Vector normal_vector(Vector v) {
  return (Vector){ -v.y, v.x };
};

Vector rotate_vector(Vector v, double angle_radians) {
  double cos_angle = cos(angle_radians);
  double sin_angle = sin(angle_radians);
  return (Vector){
    v.x * cos_angle - v.y * sin_angle,
    v.x * sin_angle + v.y * cos_angle
  };
};

double vector_length(Vector v) {
  return sqrt(vector_dot_product(v, v));
}

Vector vector_scale(Vector v, double s) {
  return (Vector){ v.x * s, v.y * s };
}

Vector vector_add(Vector a, Vector b) {
  return (Vector){ a.x + b.x, a.y + b.y };
}

double point_to_segment_distance(Vector point, Vector seg_start, Vector seg_end) {
  Vector seg = subtract_vectors(seg_end, seg_start);
  double seg_length_sq = vector_dot_product(seg, seg);

  if (seg_length_sq == 0.0) {
    return vector_length(subtract_vectors(point, seg_start));
  }

  double t = vector_dot_product(subtract_vectors(point, seg_start), seg) / seg_length_sq;
  if (t < 0.0) t = 0.0;
  else if (t > 1.0) t = 1.0;

  Vector closest = vector_add(seg_start, vector_scale(seg, t));
  return vector_length(subtract_vectors(point, closest));
}

double cross_product_2d(Vector a, Vector b) {
  return a.x * b.y - a.y * b.x;
}

double degrees_to_radians(double degrees) {
  return degrees * (M_PI / 180.0);
}

// === Separating Axis Theorem functions ===

// project all corners of a Rectangle onto an axis and get min/max
static void project_onto_axis(Rectangle rect, Vector axis, float* min, float* max) {
  *min = FLT_MAX;
  *max = -FLT_MAX;
  
  for (int i = 0; i < 4; i++) {
    float projection = vector_dot_product(rect.corner[i], axis);
    if (projection < *min) *min = projection;
    if (projection > *max) *max = projection;
  }
}

// check if projections overlap
static int projections_overlap(float min1, float max1, float min2, float max2) {
    return !(max1 < min2 || max2 < min1);
}

int separating_axis(Rectangle rect1, Rectangle rect2) {
  Rectangle rects[2] = {rect1, rect2};
  
  // check all 4 axes (2 from each rectangle)
  for (int r = 0; r < 2; r++) {
    for (int i = 0; i < 2; i++) {  // we only need 2 edges per rect (others are parallel)
      // get edge vector
      Vector edge = subtract_vectors(rects[r].corner[(i + 1) % 4], 
                              rects[r].corner[i]);

      // get perpendicular axis
      Vector axis = normal_vector(edge);

      // project both rectangles onto this axis
      float min1, max1, min2, max2;
      project_onto_axis(rect1, axis, &min1, &max1);
      project_onto_axis(rect2, axis, &min2, &max2);
      
      // if these projections don't overlap, then the rectangles don't overlap
      if (!projections_overlap(min1, max1, min2, max2)) {
        return 1;  // found a separating axis! 
      }
    }
  }
  
  return 0;  // no separating axis found, they overlap :(
}
