#include <math.h>
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
