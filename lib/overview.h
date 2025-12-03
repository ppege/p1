#pragma once
#include <data.h>
#include <display.h>

// Functions to help calculate stuff
Vector find_xy_minmax(const Lot *lot, int level);
int spots_per_level(const Lot *lot, int level);
Vector level_start_stop (const Lot *lot, int level);
int cord_calc(int x, int y, const Lot *lot, int level);
int o_height(const Lot *lot, int level);
int o_width(const Lot *lot, int level);

// Construct, modify and print the arrays
char** populated_arrays(const Lot *lot, int scale_factor);
char* populate_array(const Lot *lot, int level, int scale_factor);
char* insert_parking_spot(const Lot *lot, char** populated_arrays, int level);
void print_overview(const Lot *lot, char** populated_arrays, int level);

// Free the arrays
void overview_free(const Lot *lot, char** populated_arrays);

