#include "lot.h"
#include "data.h"
#include <stdio.h>
#include <stdlib.h>

// Function to create a Lot
Lot create_lot(int level_count, int path_count, int space_count, int up_count, int down_count) {
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
  printf("Lot Entrance: (%.2f, %.2f, Level %d)\n", lot.entrance.x, lot.entrance.y, lot.entrance.level);
  printf("Lot POI: (%.2f, %.2f, Level %d)\n", lot.POI.x, lot.POI.y, lot.POI.level);
  printf("The lot has %d levels.\n", lot.level_count);

  printf("\nPaths:\n");
  for (int i = 0; i < lot.path_count; i++) {
    printf("  Path %d: Start=(%.2f, %.2f, Level %d), Vector=(%.2f, %.2f)\n",
           i, lot.paths[i].start_point.x, lot.paths[i].start_point.y, lot.paths[i].start_point.level,
           lot.paths[i].vector.x, lot.paths[i].vector.y);
  }

  printf("\nSpaces:\n");
  for (int i = 0; i < lot.space_count; i++) {
    printf("  Space %d: Name=%s, Type=%d, Location=(%.2f, %.2f, Level %d), Rotation=%.2f°\n",
           i, lot.spaces[i].name, lot.spaces[i].type, lot.spaces[i].location.x, lot.spaces[i].location.y, lot.spaces[i].location.level, lot.spaces[i].rotation);
  }

  printf("\nUps:\n");
  for (int i = 0; i < lot.up_count; i++) {
    printf("  Up %d: (%.2f, %.2f, Level %d)\n",
           i, lot.ups[i].x, lot.ups[i].y, lot.ups[i].level);
  }

  printf("\nDowns:\n");
  for (int i = 0; i < lot.down_count; i++) {
    printf("  Down %d: (%.2f, %.2f, Level %d)\n",
           i, lot.downs[i].x, lot.downs[i].y, lot.downs[i].level);
  }
}

