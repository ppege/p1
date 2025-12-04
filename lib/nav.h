#pragma once
#include <data.h>

Location closest_point_on_path(const Path path, const Space space);
Path* available_paths(const Lot lot, const Space space, double max_distance, int* out_count);
