#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "validate.h"
#include "data.h"
#include "calculations.h"

int validate_lot(const Lot *lot) {
  // To validate a lot, the following rules must be checked:
  // 1. Each path must connect and each space must be accessible from at least one path.
  if (!paths_connected(lot)) return 0;
  // 2. Spaces must not overlap.
  if (spaces_overlap(lot)) return 0;
  // 3. No obstacles (spaces) must encroach within 1.5 meters of either side of the path centerline.
  // 4. Spaces must be within 6-7 meters of a path (to be accessible)
  // 5. There must be exactly one entrance and one POI defined.
  // 6. If the amount of levels n is greater than 1, there must be n-1 ups and n-1 downs.
  // 7. Each level must have an up and a down unless they lead to non existing levels. (each level must be accessible and reversible)
  // If all rules are satisfied, return 1 (true). Otherwise, return 0 (false).
  return 0;
}

int paths_connected(const Lot *lot) {
  int connected = 0;
  Location* endpoints = get_all_endpoints(lot->paths, lot->path_count); // get every path's endpoint
  int amount_of_endpoints = lot->path_count;
  // for each path we check:
  for (int i = 0; i < lot->path_count; i++) {
    int cur = connected;
    // is the start point identical to any endpoint?
    for (int j = 0; j < amount_of_endpoints; j++) {
      Location endpoint = endpoints[j];
      if (compare_locations(lot->paths[i].start_point, endpoint)) {
        connected++;
        break;
      }
    }
    if (cur != connected) continue;
    // no? then is the start point the entrance?
    if (compare_locations(lot->paths[i].start_point, lot->entrance)) {
      connected++;
      continue;
    }
    // no? then is the start point an up?
    for (int k = 0; k < lot->up_count; k++) {
      if (compare_locations(lot->paths[i].start_point, lot->ups[k])) {
        connected++;
        break;
      }
    }
    if (cur != connected) continue;
    // no? then is the start point a down?
    for (int l = 0; l < lot->down_count; l++) {
      if (compare_locations(lot->paths[i].start_point, lot->downs[l])) {
        connected++;
        break;
      }
    }
    // still nothing? then the path is an orphan.
    // connected will be less than path_count,
    // causing the function to return 0.
  }
  free(endpoints);
  return connected == lot->path_count; 
}

Location* get_all_endpoints(Path* paths, int path_count) {
  Location* endpoints = (Location*)malloc(sizeof(Location) * path_count);
  for (int i = 0; i < path_count; i++) {
    endpoints[i] = get_endpoint(&paths[i]);
  }
  return endpoints;
}

int compare_locations(Location loc1, Location loc2) {
    return (loc1.x == loc2.x) && (loc1.y == loc2.y) && (loc1.level == loc2.level);
}

int spaces_overlap(const Lot *lot) {
  // using get_space_rectangle we get the rectangles of each space
  for (int i = 0; i < lot->space_count; i++) {
    Rectangle rect1 = get_space_rectangle(&lot->spaces[i]);
    // we then wanna compare it to every other space's rectangle
    for (int j = i + 1; j < lot->space_count; j++) {
      Rectangle rect2 = get_space_rectangle(&lot->spaces[j]);
      // by the separating axis theorem, if we find one axis where they do not overlap, we can be sure there is no collision.
      if (!separating_axis(&rect1, &rect2)) {
        return 1; // overlap found
      }
    }
  }
  return 0; // no overlaps found
};
