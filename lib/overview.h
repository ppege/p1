#pragma once
#include <data.h>
#include <display.h>


Vector find_xy_minmax(const Lot *lot);
char* populate_array(const Lot *lot);
void print_overview(const Lot *lot, const char* parking_lot);
void overview_free(char* parking_lot);

