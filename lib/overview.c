#include "overview.h"
#include "data.h"
#include "display.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Need some sort of function to handle print logic for
// how paths or parking spots show up.

//  Function that finds upper bound for parking lot
Vector find_xy_minmax(const Lot *lot, const int level) {
  // Finds x_min, x_max, y_min, y_max for all objects that
  // has an x or y coordinate: (spaces, ups, downs, entrance, |maybe POI)
  // for a specific level.
  double x_max = 0, y_max = 0;
  for (int i = 0; i < lot->space_count; i++) {
    if (lot->spaces[i].location.level > level) {
      break;
    }
    if (lot->spaces[i].location.x > x_max &&
        lot->spaces[i].location.level == level) {
      x_max = lot->spaces[i].location.x;
    }
    if (lot->spaces[i].location.y > y_max &&
        lot->spaces[i].location.level == level) {
      y_max = lot->spaces[i].location.y;
    }
  }
  // Finds the x_min, x_max, y_min, y_max values for all objects
  // that depends on a level count.
  for (int i = 0; i <= 1; i++) {
    if (lot->ups[i].x > x_max && lot->ups[i].level == level) {
      x_max = lot->ups[i].x;
    }
    if (lot->ups[i].y > y_max && lot->ups[i].level == level) {
      y_max = lot->ups[i].y;
    }
    if (lot->downs[i].x > x_max && lot->downs[i].level == level) {
      x_max = lot->downs[i].x;
    }
    if (lot->downs[i].y > y_max && lot->downs[i].level == level) {
      y_max = lot->downs[i].y;
    }
  }
  return (Vector){ceil(x_max), ceil(y_max)};
}

// Function to translate coords into the correct elements in the array;
int cord_calc(const int x, const int y, const Lot *lot, const int level) {
  const int width = o_width(lot, level);
  return y * width + x;
}

// Help function to find amount of spaces each level
int spots_per_level(const Lot *lot, const int level) {
  int count = 0;
  for (int i = 0; i < lot->space_count; i++) {
    if (lot->spaces[i].location.level == level) {
      count++;
    }
  }
  printf("%d", count);
  return count;
}

// Help function to calc when spaces start and stop each level
Vector level_start_stop(const Lot *lot, const int level){
  int start = 0, stop = 0;
  for (int i = 0; i < lot->space_count; i++) {
    if (lot->spaces[i].location.level == level) {
      start = i;
      break;
    }
  }
  for (int i = 0; i < lot->space_count; i++) {
  if (lot->spaces[i].location.level > level) {
    break;
  }
    stop = i;
  }
  return (Vector){start, stop};
}

// Calculates height and width of parking lot
int o_height(const Lot *lot, const int level) {
  const Vector max = find_xy_minmax(lot, level);
  // +1 so it goas up to the max of 12 instead of 0 to 11
  return (int)max.y+1;
}
int o_width(const Lot *lot, const int level) {
  const Vector max = find_xy_minmax(lot, level);
  // +1 so it goas up to the max of 12 instead of 0 to 11
  return (int)max.x+1;
}

// Creates an array of pointers, so it is easy to acces the overview
// for each level.
char **populated_arrays(const Lot *lot, const int scale_factor) {
  const int levels = lot->level_count;
  char **levels_overview = malloc(levels * sizeof(char *));
  for (int i = 0; i < levels; i++) {
    levels_overview[i] = populate_array(lot, i, scale_factor);
  }
  return levels_overview;
}

// Make dynamic array for each level
// Function to create a dynamic array for a given level
char *populate_array(const Lot *lot, const int level, const int scale_factor) {
  const int height = scale_factor*o_height(lot, level);
  const int width = scale_factor*o_width(lot, level);
  const int size_of_lot = (width * height);
  char *parking_lot = calloc(size_of_lot, sizeof(char));

  // For loop that creates the border around the parking lot
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      parking_lot[width * i + j] = 'x';
    }
  }
  return parking_lot;
}

// Function to insert a parking spot placed by lower x and y value
char* insert_parking_spot(const Lot *lot, char **populated_arrays, const int level) {
  char* parking_lot = populated_arrays[level];
  // .x is the first value for a parking spot on a given level and .y is the last.
  const Vector spots = level_start_stop(lot, level);
  // Parking spaces, defined by what type it is.
  for (int i = (int)spots.x; i <= (int)spots.y; i++) {
    switch (lot->spaces->type) {
      case Standard:
        parking_lot[cord_calc(ceil(lot->spaces[i].location.x), ceil(lot->spaces[i].location.y),lot,level)] = '0';
        break;
      case Handicap:
        parking_lot[cord_calc(ceil(lot->spaces[i].location.x), ceil(lot->spaces[i].location.y),lot,level)] = '1';
        break;
      case Compact:
        parking_lot[cord_calc(ceil(lot->spaces[i].location.x), ceil(lot->spaces[i].location.y),lot,level)] = '2';
        break;
      case EV:
        parking_lot[cord_calc(ceil(lot->spaces[i].location.x), ceil(lot->spaces[i].location.y),lot,level)] = '3';
        break;
    }
  }
  //entrance
  if (lot->entrance.level == level) {
    parking_lot[cord_calc(ceil(lot->entrance.x), ceil(lot->entrance.y),lot,level)] = 'e';
  }

  // Ups & Downs
  if (lot->up_count>0) {
    for (int i = 0; i < lot->up_count; i++) {
      if (lot->ups[i].level == level){
        parking_lot[cord_calc(ceil(lot->ups[i].x), ceil(lot->ups[i].x),lot,level)]= 'u';
      }
    }
  }
  if (lot->down_count>0) {
    for (int i = 0; i < lot->up_count; i++) {
      if (lot->downs[i].level == level){
        parking_lot[cord_calc(ceil(lot->downs[i].x), ceil(lot->downs[i].x),lot,level)]= 'd';
      }
    }
  }

  // Paths
 /* for (int i = 0; i < lot->path_count; i++) {
    if (lot->paths->start_point.level == level) {

    }
  }*/
  return parking_lot;
}

// Function to calc and scale how the overview should look
char* define_parking_spot(const Lot *lot, char **populated_arrays, const int level) {
  // scales all the stuff in the array, so it is possible to make 2*2 or 3*3 tiles for it
  const int height = o_height(lot, level);
  const int width = o_width(lot, level);
  // First it scales on the x axis





}



// The function responsible for printing the overview of the parking lot
void print_overview(const Lot *lot, char **populated_arrays, const int level) {
  const char *parking_lot = populated_arrays[level];
  const int height = o_height(lot, level);
  const int width = o_width(lot, level);
  int wrap = width * height - width;
  clear_screen();
  // Loop that defines how tall the parking space is:
  for (int i = 0; i < (height+1); i++) {
    if (i == 0) {
      BoxStart(width);
    }
    for (int j = 0; j < (width+2); j++) {
      if (j == 0 && i < height) {
        printf("│");
      }
      if (j >= 0 && j < width && i >= 0 && i < height) {
        // Make a switch case here to print the correct characters

        printf("%c", parking_lot[wrap + j]);
      }
      if (j == width+1 && i < height) {
        printf("│");
      }
    }
    wrap -= width;
    if (i==height){
      BoxEnd(width);
    }
    printf("\n");
  }
}

// Frees the allocated memory
void overview_free(const Lot *lot, char **populated_arrays) {
  const int levels = lot->level_count;
  for (int i = 0; i < levels; i++) {
    free(populated_arrays[i]);
  }
  free(populated_arrays);
}
