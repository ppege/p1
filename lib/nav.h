#pragma once
#include <data.h>

Path* superpath_to_space(const Lot lot, const Space space, int* out_count);
double superpath_length(const Path* superpath, int count);
