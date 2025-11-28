#pragma once
#include <data.h>

Vector subtract_vectors(Vector v1, Vector v2);
double vector_dot_product(Vector v1, Vector v2);
Vector normal_vector(Vector v);
Vector rotate_vector(Vector v, double angle_radians);
double degrees_to_radians(double degrees);
