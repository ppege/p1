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

double degrees_to_radians(double degrees) {
  return degrees * (M_PI / 180.0);
}

// === Separating Axis Theorem functions ===

// project all corners of a Rectangle onto an axis and get min/max
void project_onto_axis(Rectangle* rect, Vector axis, float* min, float* max) {
  *min = FLT_MAX;
  *max = -FLT_MAX;
  
  for (int i = 0; i < 4; i++) {
    float projection = vector_dot_product(rect->corner[i], axis);
    if (projection < *min) *min = projection;
    if (projection > *max) *max = projection;
  }
}

// check if projections overlap
int projections_overlap(float min1, float max1, float min2, float max2) {
    return !(max1 < min2 || max2 < min1);
}

int separating_axis(Rectangle* rect1, Rectangle* rect2) {
  Rectangle* rects[2] = {rect1, rect2};
  
  // check all 4 axes (2 from each rectangle)
  for (int r = 0; r < 2; r++) {
    for (int i = 0; i < 2; i++) {  // we only need 2 edges per rect (others are parallel)
      // get edge vector
      Vector edge = subtract_vectors(rects[r]->corner[(i + 1) % 4], 
                              rects[r]->corner[i]);

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
