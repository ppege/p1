#pragma once
#include <data.h>

Vector subtract_vectors(Vector a, Vector b);
double vector_dot_product(Vector a, Vector b);
Vector normal_vector(Vector v);
Vector rotate_vector(Vector v, double angle_radians);
double vector_length(Vector v);

Vector vector_scale(Vector v, double s);
Vector vector_add(Vector a, Vector b);
double point_to_segment_distance(Vector point, Vector seg_start, Vector seg_end);
double cross_product_2d(Vector a, Vector b);
double degrees_to_radians(double degrees);

int separating_axis(Rectangle rect1, Rectangle rect2);
