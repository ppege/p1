#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "validate.h"
#include "data.h"
#include "calculations.h"

#define PATH_CLEARANCE 1.5
#define PATH_ACCESSIBILITY 6.0

int validate_lot(const Lot *lot) {
  // To validate a lot, the following rules must be checked:
  // 0. Each path must have a non-zero length. Checking this is trivial:
  for (int i = 0; i < lot->path_count; i++) {
    double path_length = sqrt(lot->paths[i].vector.x * lot->paths[i].vector.x + lot->paths[i].vector.y * lot->paths[i].vector.y);
    if (path_length < DBL_EPSILON) {
      return 0; // invalid path found
    }
  }
  // 1. Each path must connect and each space must be accessible from at least one path.
  if (!paths_connected(lot)) return 0;
  // 2. Spaces must not overlap.
  if (spaces_overlap(lot)) return 0;
  // 3. No obstacles (spaces) must encroach within PATH_CLEARANCE meters of either side of the path centerline.
  if (spaces_encroach_path(lot, PATH_CLEARANCE)) return 0;
  // 4. Spaces must be within 6-7 meters of a path (to be accessible)
  if (!spaces_accessible(lot, PATH_ACCESSIBILITY)) return 0;
  // 5. There must be exactly one entrance and one POI defined.
  // 6. Every space must have a unique name.
  // 7. If the amount of levels n is greater than 1, there must be n-1 ups and n-1 downs.
  // 8. Each level must have an up and a down unless they lead to non existing levels. (each level must be accessible and reversible)
  // If all rules are satisfied, return 1 (true). Otherwise, return 0 (false).
  return 1;
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

int spaces_encroach_path(const Lot *lot, double margin) {
  // for each path, we create a "corridor" rectangle representing the path with margin on both sides. 
  // then we check if any space's rectangle overlaps with this corridor.
  
  // for each path
  for (int i = 0; i < lot->path_count; i++) {
    Rectangle path_corridor = get_path_corridor(&lot->paths[i], margin);
    
    // check against each space
    for (int j = 0; j < lot->space_count; j++) {
      // only check spaces on the same level as the path
      if (lot->spaces[j].location.level != lot->paths[i].start_point.level) { continue; }
      
      Rectangle space_rect = get_space_rectangle(&lot->spaces[j]);
      
      // if no separating axis is found, the rectangles overlap
      if (!separating_axis(&path_corridor, &space_rect)) {
        return 1; // encroachment found
      }
    }
  }
  return 0; // no encroachments found
}

Rectangle get_path_corridor(const Path *path, double margin) {
  // here, we create a rectangle that represents a path with the margin on both sides.
  
  // first we have to get the normalized perpendicular vector to the path direction
  double path_length = sqrt(path->vector.x * path->vector.x + path->vector.y * path->vector.y);
  Vector dir = {path->vector.x / path_length, path->vector.y / path_length};
  Vector normal = normal_vector(dir);
  
  // Scale normal by margin
  Vector offset = {normal.x * margin, normal.y * margin};
  
  // Calculate the four corners of the corridor rectangle:
  // Starting from start_point, going to endpoint (start + vector)
  // Offset by margin in the normal direction on both sides
  Rectangle rect;
  
  // Corner 0: start - offset
  rect. corner[0] = (Vector){
    path->start_point. x - offset.x,
    path->start_point.y - offset.y
  };

  // Corner 1: end - offset (follow the path direction first!)
  rect.corner[1] = (Vector){
    path->start_point.x + path->vector.x - offset. x,
    path->start_point. y + path->vector.y - offset. y
  };

  // Corner 2: end + offset
  rect. corner[2] = (Vector){
    path->start_point.x + path->vector.x + offset.x,
    path->start_point.y + path->vector.y + offset.y
  };

  // Corner 3: start + offset
  rect.corner[3] = (Vector){
    path->start_point.x + offset.x,
    path->start_point.y + offset.y
  };
  
  return rect;
}

int spaces_accessible(const Lot *lot, double max_distance) {
  // for each space, check if it overlaps with at least one path's accessibility corridor
  for (int i = 0; i < lot->space_count; i++) {
    Rectangle space_rect = get_space_rectangle(&lot->spaces[i]);
    int accessible = 0;
    
    // check against each path
    for (int j = 0; j < lot->path_count; j++) {
      // only check paths on the same level as the space
      if (lot->spaces[i].location.level != lot->paths[j].start_point.level) { continue; }
      
      Rectangle path_corridor = get_path_corridor(&lot->paths[j], max_distance);
      
      // if no separating axis found → space overlaps with corridor → accessible
      if (!separating_axis(&path_corridor, &space_rect)) {
        accessible = 1;
        break;
      }
    }
    
    if (!accessible) {
      return 0; // this space is not accessible from any path
    }
  }
  return 1; // all spaces are accessible
}
