#include "lot.h"
#include "data.h"
#include <stdio.h>
#include <stdlib.h>

// Function to create a Lot
Lot* create_lot(int path_count, int space_count, int up_count, int down_count) {
  Lot* lot = malloc(sizeof(Lot));
  lot->paths = malloc(path_count * sizeof(Path));
  lot->spaces = malloc(space_count * sizeof(Space));
  lot->ups = malloc(up_count * sizeof(Location));
  lot->downs = malloc(down_count * sizeof(Location));
  lot->path_count = path_count;
  lot->space_count = space_count;
  lot->up_count = up_count;
  lot->down_count = down_count;
  return lot;
}

// Function to free a Lot
void free_lot(Lot* lot) {
  free(lot->paths);
  free(lot->spaces);
  free(lot->ups);
  free(lot->downs);
  free(lot);
}

// Function to print a Lot
void print_lot(const Lot* lot) {
  printf("Lot Entrance: (%.2f, %.2f, Level %d)\n", lot->entrance.x, lot->entrance.y, lot->entrance.level);
  printf("Lot POI: (%.2f, %.2f, Level %d)\n", lot->POI.x, lot->POI.y, lot->POI.level);

  printf("\nPaths:\n");
  for (int i = 0; i < lot->path_count; i++) {
    printf("  Path %d: Start=(%.2f, %.2f, Level %d), Vector=(%.2f, %.2f)\n",
           i, lot->paths[i].start_point.x, lot->paths[i].start_point.y, lot->paths[i].start_point.level,
           lot->paths[i].vector.x, lot->paths[i].vector.y);
  }

  printf("\nSpaces:\n");
  for (int i = 0; i < lot->space_count; i++) {
    printf("  Space %d: Name=%s, Type=%d, Location=(%.2f, %.2f, Level %d)\n",
           i, lot->spaces[i].name, lot->spaces[i].type, lot->spaces[i].location.x, lot->spaces[i].location.y, lot->spaces[i].location.level);
  }

  printf("\nUps:\n");
  for (int i = 0; i < lot->up_count; i++) {
    printf("  Up %d: (%.2f, %.2f, Level %d)\n",
           i, lot->ups[i].x, lot->ups[i].y, lot->ups[i].level);
  }

  printf("\nDowns:\n");
  for (int i = 0; i < lot->down_count; i++) {
    printf("  Down %d: (%.2f, %.2f, Level %d)\n",
           i, lot->downs[i].x, lot->downs[i].y, lot->downs[i].level);
  }
}

void populate_lot(Lot* lot) {
  lot->entrance = (Location){7.0, 1.0, 0};
  lot->POI = (Location){7.0, -5.0, 0};
  lot->paths[0] = (Path){.vector = {0.0, 9.0}, .start_point = {7.0, 1.0, 0}};
  lot->paths[1] = (Path){.vector = {12.0, 0.0}, .start_point = {1.0, 2.0, 0}};
  lot->paths[2] = (Path){.vector = {12.0, 0.0}, .start_point = {1.0, 7.0, 0}};
  lot->paths[3] = (Path){.vector = {0.0, 9.0}, .start_point = {7.0, 1.0, 1}};
  lot->paths[4] = (Path){.vector = {12.0, 0.0}, .start_point = {1.0, 2.0, 1}};
  lot->paths[5] = (Path){.vector = {12.0, 0.0}, .start_point = {1.0, 7.0, 1}};

  lot->ups[0] = (Location){7.0, 10.0, 0};
  lot->downs[0] = (Location){7.0, 10.0, 1};
  
  lot->spaces[0]  = (Space){.type = Standard, .location = {1.0, 2.5, 0}, .name = "A1"};
  lot->spaces[1]  = (Space){.type = Standard, .location = {2.0, 2.5, 0}, .name = "A2"};
  lot->spaces[2]  = (Space){.type = Standard, .location = {3.0, 2.5, 0}, .name = "A3"};
  lot->spaces[3]  = (Space){.type = Standard, .location = {4.0, 2.5, 0}, .name = "A4"};
  lot->spaces[4]  = (Space){.type = Standard, .location = {5.0, 2.5, 0}, .name = "A5"};
  lot->spaces[5]  = (Space){.type = Standard, .location = {8.0, 2.5, 0}, .name = "A6"};
  lot->spaces[6]  = (Space){.type = Standard, .location = {9.0, 2.5, 0}, .name = "A7"};
  lot->spaces[7]  = (Space){.type = Standard, .location = {10.0, 2.5, 0}, .name = "A8"};
  lot->spaces[8]  = (Space){.type = Standard, .location = {11.0, 2.5, 0}, .name = "A9"};
  lot->spaces[9]  = (Space){.type = Standard, .location = {12.0, 2.5, 0}, .name = "A10"};
  lot->spaces[10] = (Space){.type = Standard, .location = {1.0, 4.5, 0}, .name = "B1"};
  lot->spaces[11] = (Space){.type = Standard, .location = {2.0, 4.5, 0}, .name = "B2"};
  lot->spaces[12] = (Space){.type = Standard, .location = {3.0, 4.5, 0}, .name = "B3"};
  lot->spaces[13] = (Space){.type = Standard, .location = {4.0, 4.5, 0}, .name = "B4"};
  lot->spaces[14] = (Space){.type = Standard, .location = {5.0, 4.5, 0}, .name = "B5"};
  lot->spaces[15] = (Space){.type = Standard, .location = {8.0, 4.5, 0}, .name = "B6"};
  lot->spaces[16] = (Space){.type = Standard, .location = {9.0, 4.5, 0}, .name = "B7"};
  lot->spaces[17] = (Space){.type = Standard, .location = {10.0, 4.5, 0}, .name = "B8"};
  lot->spaces[18] = (Space){.type = Standard, .location = {11.0, 4.5, 0}, .name = "B9"};
  lot->spaces[19] = (Space){.type = Standard, .location = {12.0, 4.5, 0}, .name = "B10"};
  lot->spaces[20] = (Space){.type = Standard, .location = {1.0, 7.5, 0}, .name = "C1"};
  lot->spaces[21] = (Space){.type = Standard, .location = {2.0, 7.5, 0}, .name = "C2"};
  lot->spaces[22] = (Space){.type = Standard, .location = {3.0, 7.5, 0}, .name = "C3"};
  lot->spaces[23] = (Space){.type = Standard, .location = {4.0, 7.5, 0}, .name = "C4"};
  lot->spaces[24] = (Space){.type = Standard, .location = {5.0, 7.5, 0}, .name = "C5"};
  lot->spaces[25] = (Space){.type = Standard, .location = {8.0, 7.5, 0}, .name = "C6"};
  lot->spaces[26] = (Space){.type = Standard, .location = {9.0, 7.5, 0}, .name = "C7"};
  lot->spaces[27] = (Space){.type = Standard, .location = {10.0, 7.5, 0}, .name = "C8"};
  lot->spaces[28] = (Space){.type = Standard, .location = {11.0, 7.5, 0}, .name = "C9"};
  lot->spaces[29] = (Space){.type = Standard, .location = {12.0, 7.5, 0}, .name = "C10"};

  lot->spaces[30] = (Space){.type = Standard, .location = {1.0, 2.5, 1}, .name = "A1"};
  lot->spaces[31] = (Space){.type = Standard, .location = {2.0, 2.5, 1}, .name = "A2"};
  lot->spaces[32] = (Space){.type = Standard, .location = {3.0, 2.5, 1}, .name = "A3"};
  lot->spaces[33] = (Space){.type = Standard, .location = {4.0, 2.5, 1}, .name = "A4"};
  lot->spaces[34] = (Space){.type = Standard, .location = {5.0, 2.5, 1}, .name = "A5"};
  lot->spaces[35] = (Space){.type = Standard, .location = {8.0, 2.5, 1}, .name = "A6"};
  lot->spaces[36] = (Space){.type = Standard, .location = {9.0, 2.5, 1}, .name = "A7"};
  lot->spaces[37] = (Space){.type = Standard, .location = {10.0, 2.5, 1}, .name = "A8"};
  lot->spaces[38] = (Space){.type = Standard, .location = {11.0, 2.5, 1}, .name = "A9"};
  lot->spaces[39] = (Space){.type = Standard, .location = {12.0, 2.5, 1}, .name = "A10"};
  lot->spaces[40] = (Space){.type = Standard, .location = {1.0, 4.5, 1}, .name = "B1"};
  lot->spaces[41] = (Space){.type = Standard, .location = {2.0, 4.5, 1}, .name = "B2"};
  lot->spaces[42] = (Space){.type = Standard, .location = {3.0, 4.5, 1}, .name = "B3"};
  lot->spaces[43] = (Space){.type = Standard, .location = {4.0, 4.5, 1}, .name = "B4"};
  lot->spaces[44] = (Space){.type = Standard, .location = {5.0, 4.5, 1}, .name = "B5"};
  lot->spaces[45] = (Space){.type = Standard, .location = {8.0, 4.5, 1}, .name = "B6"};
  lot->spaces[46] = (Space){.type = Standard, .location = {9.0, 4.5, 1}, .name = "B7"};
  lot->spaces[47] = (Space){.type = Standard, .location = {10.0, 4.5, 1}, .name = "B8"};
  lot->spaces[48] = (Space){.type = Standard, .location = {11.0, 4.5, 1}, .name = "B9"};
  lot->spaces[49] = (Space){.type = Standard, .location = {12.0, 4.5, 1}, .name = "B10"};
  lot->spaces[50] = (Space){.type = Standard, .location = {1.0, 7.5, 1}, .name = "C1"};
  lot->spaces[51] = (Space){.type = Standard, .location = {2.0, 7.5, 1}, .name = "C2"};
  lot->spaces[52] = (Space){.type = Standard, .location = {3.0, 7.5, 1}, .name = "C3"};
  lot->spaces[53] = (Space){.type = Standard, .location = {4.0, 7.5, 1}, .name = "C4"};
  lot->spaces[54] = (Space){.type = Standard, .location = {5.0, 7.5, 1}, .name = "C5"};
  lot->spaces[55] = (Space){.type = Standard, .location = {8.0, 7.5, 1}, .name = "C6"};
  lot->spaces[56] = (Space){.type = Standard, .location = {9.0, 7.5, 1}, .name = "C7"};
  lot->spaces[57] = (Space){.type = Standard, .location = {10.0, 7.5, 1}, .name = "C8"};
  lot->spaces[58] = (Space){.type = Standard, .location = {11.0, 7.5, 1}, .name = "C9"};
  lot->spaces[59] = (Space){.type = Standard, .location = {12.0, 7.5, 1}, .name = "C10"};
} 
