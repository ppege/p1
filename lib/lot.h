#pragma once
#include <data.h>

Lot create_lot(int level_count, int path_count, int space_count, int up_count, int down_count);
void free_lot(Lot lot);
void print_lot(const Lot lot);

Space* space_by_name(const Lot lot, const char* name);
int count_levels(const Lot lot);
Space* best_space(const Lot lot, SpaceType type);
