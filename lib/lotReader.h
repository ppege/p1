#pragma once
#include "data.h"

Lot lot_from_file(char *filename);
Space* readSpace(char *line);
Path* readPath(char *line);
Location* readLocation(char *line);
