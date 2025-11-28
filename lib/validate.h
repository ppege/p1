#pragma once
#include <data.h>

int validate_lot(const Lot *lot);
int paths_connected(const Lot *lot);
Location* get_all_endpoints(Path* paths, int path_count);
int compare_locations(Location loc1, Location loc2);
int spaces_overlap(const Lot *lot);
