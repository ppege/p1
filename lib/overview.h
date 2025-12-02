#pragma once
#include <data.h>
#include <display.h>


Vector find_xy_minmax(const Lot *lot, int level);

int cord_calc(const int x, const int y, const Lot *lot, const int level);

int o_height(const Lot *lot, int level);

int o_width(const Lot *lot, int level);

char* populate_array(const Lot *lot, int level);

void print_overview(const Lot *lot, const char* parking_lot, int level);

char** populated_arrays(const Lot *lot);

void overview_free(const Lot *lot, char** populated_arrays);

