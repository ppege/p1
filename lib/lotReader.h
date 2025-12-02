#pragma once
#include "data.h"

Space readSpace(char *line);
Path readPath(char *line);
Location readLocation(char *line);
void readLotFromFile(char *filename, Lot *lot);
