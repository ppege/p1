#include "lot.h"
#include "data.h"
#include "nav.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to create a Lot
Lot create_lot(int level_count, int path_count, int space_count, int up_count,
               int down_count) {
  Lot lot;
  lot.level_count = level_count;
  lot.paths = malloc(path_count * sizeof(Path));
  lot.spaces = malloc(space_count * sizeof(Space));
  lot.ups = malloc(up_count * sizeof(Location));
  lot.downs = malloc(down_count * sizeof(Location));
  lot.path_count = path_count;
  lot.space_count = space_count;
  lot.up_count = up_count;
  lot.down_count = down_count;
  return lot;
}

// Function to free a Lot
void free_lot(Lot lot) {
  free(lot.paths);
  free(lot.spaces);
  free(lot.ups);
  free(lot.downs);
}

// Function to print a Lot
void print_lot(const Lot lot) {
  printf("Lot Entrance: (%.2f, %.2f, Level %d)\n", lot.entrance.x,
         lot.entrance.y, lot.entrance.level);
  printf("Lot POI: (%.2f, %.2f, Level %d)\n", lot.POI.x, lot.POI.y,
         lot.POI.level);
  printf("The lot has %d levels.\n", lot.level_count);

  printf("\nPaths:\n");
  for (int i = 0; i < lot.path_count; i++) {
    printf("  Path %d: Start=(%.2f, %.2f, Level %d), Vector=(%.2f, %.2f)\n", i,
           lot.paths[i].start_point.x, lot.paths[i].start_point.y,
           lot.paths[i].start_point.level, lot.paths[i].vector.x,
           lot.paths[i].vector.y);
  }

  printf("\nSpaces:\n");
  for (int i = 0; i < lot.space_count; i++) {
    printf("  Space %d: Name=%s, Type=%d, Location=(%.2f, %.2f, Level %d), "
           "Rotation=%.2f°\n",
           i, lot.spaces[i].name, lot.spaces[i].type, lot.spaces[i].location.x,
           lot.spaces[i].location.y, lot.spaces[i].location.level,
           lot.spaces[i].rotation);
  }

  printf("\nUps:\n");
  for (int i = 0; i < lot.up_count; i++) {
    printf("  Up %d: (%.2f, %.2f, Level %d)\n", i, lot.ups[i].x, lot.ups[i].y,
           lot.ups[i].level);
  }

  printf("\nDowns:\n");
  for (int i = 0; i < lot.down_count; i++) {
    printf("  Down %d: (%.2f, %.2f, Level %d)\n", i, lot.downs[i].x,
           lot.downs[i].y, lot.downs[i].level);
  }
}

Space *space_by_name(const Lot lot, const char *name) {
  for (int i = 0; i < lot.space_count; i++) {
    if (strcmp(lot.spaces[i].name, name) == 0) {
      return &lot.spaces[i];
    }
  }
  return NULL;
}

int count_levels(const Lot lot) {
  // to find the level count we must find the number of unique levels in paths
  // and spaces
  int level_count = 0;
  // this ensures we have space even if EVERY location in the lot is a unique
  // level lmao
  int *levels = malloc(sizeof(int) * (lot.space_count + lot.path_count +
                                      lot.up_count + lot.down_count));
  for (int i = 0; i < lot.space_count; i++) {
    int level = lot.spaces[i].location.level;
    // check if level is already in levels
    int found = 0;
    for (int j = 0; j < level_count; j++) {
      if (levels[j] == level) {
        found = 1;
        break;
      }
    }
    if (!found) {
      levels[level_count++] = level;
    }
  }
  for (int i = 0; i < lot.path_count; i++) {
    int level = lot.paths[i].start_point.level;
    // check if level is already in levels
    int found = 0;
    for (int j = 0; j < level_count; j++) {
      if (levels[j] == level) {
        found = 1;
        break;
      }
    }
    if (!found) {
      levels[level_count++] = level;
    }
  }
  for (int i = 0; i < lot.up_count; i++) {
    int level = lot.ups[i].level;
    // check if level is already in levels
    int found = 0;
    for (int j = 0; j < level_count; j++) {
      if (levels[j] == level) {
        found = 1;
        break;
      }
    }
    if (!found) {
      levels[level_count++] = level;
    }
  }
  for (int i = 0; i < lot.down_count; i++) {
    int level = lot.downs[i].level;
    // check if level is already in levels
    int found = 0;
    for (int j = 0; j < level_count; j++) {
      if (levels[j] == level) {
        found = 1;
        break;
      }
    }
    if (!found) {
      levels[level_count++] = level;
    }
  }
  free(levels);
  return level_count;
}

// find the best available space of a given type; best means closest to the
// entrance
Space *best_space(const Lot lot, SpaceType type) {
  Space *best = NULL;
  double best_distance = -1.0;

  for (int i = 0; i < lot.space_count; i++) {
    // check if space is of the desired type and unoccupied
    if (lot.spaces[i].type != type || lot.spaces[i].occupied != -1) {
      continue;
    }

    printf("Evaluating space %s at (%.2f, %.2f, Level %d)\n",
           lot.spaces[i].name, lot.spaces[i].location.x,
           lot.spaces[i].location.y, lot.spaces[i].location.level);

    // calculate distance from entrance
    int count = 0;
    Path *superpath = superpath_to_space(lot, lot.spaces[i], &count);
    if (count == -1) {
      continue; // no valid path to this space
    }
    double distance = superpath_length(superpath, count);
    free(superpath);

    // add ramp length * level difference (if 0 nothing is added)
    int level_diff = abs(lot.spaces[i].location.level - lot.entrance.level);
    distance += level_diff * lot.ramp_length;

    // check if this is the best (shortest) so far
    if (best_distance < 0 || distance < best_distance) {
      best_distance = distance;
      best = &lot.spaces[i];
    }
  }
  return best;
}
