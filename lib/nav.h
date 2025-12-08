#pragma once
#include <data.h>

Location closest_point_on_path(const Path path, const Space space);
Path* available_paths(const Lot lot, const Space space, double max_distance, int* out_count);
Path get_subpath(const Path path, const Location endpoint);
Path get_turnpath(const Location subpath_endpoint, const Space destination_space);
Path* superpath_to_space(const Lot lot, const Space space, int* out_count);
