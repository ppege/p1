#include "validate.h"
#include "calculations.h"
#include "data.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// macros to help return results
#define OK (ValidationResult){ Ok, NoError }
#define ERR(e) (ValidationResult){ Err, e }

// map errors to messages
const char *validation_error_message(LotValidationError error) {
  switch (error) {
    case NoError:                  return "No error";
    case ZeroLengthPath:           return "A path has zero length";
    case PathNotConnected:         return "Paths are not all connected";
    case SpacesOverlap:            return "Some spaces overlap";
    case SpacesEncroachPath:       return "A space encroaches on a path";
    case SpacesInaccessible:       return "A space is not accessible from any path";
    case InvalidEntranceOrPOI:     return "Entrance or POI is on an invalid level";
    case DuplicateSpaceNames:      return "Duplicate space names found";
    case IncorrectUpDownCount:     return "Incorrect number of ups/downs for level count";
    case LevelsMissingUpsOrDowns:  return "Some levels are missing required ups or downs";
    case SpaceNameTooLong:         return "A space name exceeds the maximum length of 10 characters";
    default:                       return "Unknown error";
  }
}

// main validation function; sequentially checks each rule
ValidationResult validate_lot(const Lot lot) {
  // Rule 0: Each path must have a non-zero length
  for (int i = 0; i < lot.path_count; i++) {
    double path_length = sqrt(lot.paths[i].vector.x * lot.paths[i].vector.x + lot.paths[i].vector.y * lot.paths[i].vector.y);
    if (path_length < DBL_EPSILON) {
      return ERR(ZeroLengthPath);
    }
  }

  // Rule 1: Each path must connect
  if (!paths_connected(lot))
    return ERR(PathNotConnected);

  // Rule 2: Spaces must not overlap
  if (spaces_overlap(lot))
    return ERR(SpacesOverlap);

  // Rule 3: No spaces encroach within PATH_CLEARANCE of path centerline
  if (spaces_encroach_path(lot, path_clearance)) return ERR(SpacesEncroachPath);
  
  // Rule 4: Spaces must be within PATH_ACCESSIBILITY of a path
  if (!spaces_accessible(lot, path_accessibility)) return ERR(SpacesInaccessible);
  
  // Rule 5: Must have valid entrance and POI
  if (!has_valid_entrance_and_poi(lot))
    return ERR(InvalidEntranceOrPOI);

  // Rule 6: Every space must have a unique name
  if (!spaces_have_unique_names(lot))
    return ERR(DuplicateSpaceNames);

  // Rule 7: Must have correct number of ups and downs
  if (!has_correct_up_down_count(lot))
    return ERR(IncorrectUpDownCount);

  // Rule 8: Each level must have appropriate ups and downs
  if (!levels_have_ups_and_downs(lot)) return ERR(LevelsMissingUpsOrDowns);

  // Rule 9: Space name length must not exceed 10 characters
  for (int i = 0; i < lot.space_count; i++) {
    if (strlen(lot.spaces[i].name) > 10) {
      return ERR(SpaceNameTooLong);
    }
  }
  
  return OK;
}

// helper function to check if all paths are connected
int paths_connected(const Lot lot) {
  int connected = 0;
  Location* endpoints = get_all_endpoints(lot.paths, lot.path_count); // get every path's endpoint
  int amount_of_endpoints = lot.path_count;
  // for each path we check:
  for (int i = 0; i < lot.path_count; i++) {
    int cur = connected;
    // is the start point identical to any endpoint?
    for (int j = 0; j < amount_of_endpoints; j++) {
      Location endpoint = endpoints[j];
      if (compare_locations(lot.paths[i].start_point, endpoint)) {
        connected++;
        break;
      }
    }
    if (cur != connected)
      continue;
    // no? then is the start point the entrance?
    if (compare_locations(lot.paths[i].start_point, lot.entrance)) {
      connected++;
      continue;
    }
    // no? then is the start point an up?
    for (int k = 0; k < lot.up_count; k++) {
      if (compare_locations(lot.paths[i].start_point, lot.ups[k])) {
        connected++;
        break;
      }
    }
    if (cur != connected)
      continue;
    // no? then is the start point a down?
    for (int l = 0; l < lot.down_count; l++) {
      if (compare_locations(lot.paths[i].start_point, lot.downs[l])) {
        connected++;
        break;
      }
    }
    // still nothing? then the path is an orphan.
    // connected will be less than path_count,
    // causing the function to return 0.
  }

  // then, for every up and down, we need to check they connect to at least one path.
  // this can either be "to" or "from" the up/down since non-entrance levels tend to have
  // paths exclusively going from ups/downs rather than to them.
  
  // for each up
  for (int m = 0; m < lot.up_count; m++) {
    // is there at least one path connecting to this up?
    for (int n = 0; n < lot.path_count; n++) {
      // first check against every endpoint
      Location endpoint = endpoints[n];
      if (compare_locations(lot.ups[m], endpoint)) {
        connected++;
        break;
      }
      // then check against every start point
      Location startpoint = lot.paths[n].start_point;
      if (compare_locations(lot.ups[m], startpoint)) {
        connected++;
        break;
      }
    }
  }
  for (int o = 0; o < lot.down_count; o++) {
    // is there at least one path connecting to this down?
    for (int p = 0; p < lot.path_count; p++) {
      Location endpoint = endpoints[p];
      if (compare_locations(lot.downs[o], endpoint)) {
        connected++;
        break;
      }
      // then check against every start point
      Location startpoint = lot.paths[p].start_point;
      if (compare_locations(lot.downs[o], startpoint)) {
        connected++;
        break;
      }
    }
  }
  free(endpoints);
  return connected == lot.path_count + lot.up_count + lot.down_count; 
}

// helper function to get all path endpoints
Location *get_all_endpoints(Path *paths, int path_count) {
  Location *endpoints = (Location *)malloc(sizeof(Location) * path_count);
  for (int i = 0; i < path_count; i++) {
    endpoints[i] = get_endpoint(paths[i]);
  }
  return endpoints;
}

// helper function to check if two rectangles are separated along any axis
// returns 1 if separated, 0 if overlapping
int spaces_overlap(const Lot lot) {
  // using get_space_rectangle we get the rectangles of each space
  for (int i = 0; i < lot.space_count; i++) {
    Rectangle rect1 = get_space_rectangle(lot.spaces[i]);
    // we then wanna compare it to every other space's rectangle
    for (int j = i + 1; j < lot.space_count; j++) {
      if (lot.spaces[i].location.level != lot.spaces[j].location.level) { continue; } // only compare spaces on the same level
      Rectangle rect2 = get_space_rectangle(lot.spaces[j]);
      // by the separating axis theorem, if we find one axis where they do not overlap, we can be sure there is no collision.
      if (!separating_axis(rect1, rect2)) {
        return 1; // overlap found
      }
    }
  }
  return 0; // no overlaps found
};

// helper function to check if any spaces encroach within margin of any path
int spaces_encroach_path(const Lot lot, double margin) {
  // for each path, we create a "corridor" rectangle representing the path with margin on both sides. 
  // then we check if any space's rectangle overlaps with this corridor.
  
  // for each path
  for (int i = 0; i < lot.path_count; i++) {
    Rectangle path_corridor = get_path_corridor(lot.paths[i], margin);
    
    // check against each space
    for (int j = 0; j < lot.space_count; j++) {
      // only check spaces on the same level as the path
      if (lot.spaces[j].location.level != lot.paths[i].start_point.level) { continue; }
      
      Rectangle space_rect = get_space_rectangle(lot.spaces[j]);
      
      // if no separating axis is found, the rectangles overlap
      if (!separating_axis(path_corridor, space_rect)) {
        return 1; // encroachment found
      }
    }
  }
  return 0; // no encroachments found
}

// helper function to get the corridor rectangle for a path with margin
Rectangle get_path_corridor(const Path path, double margin) {
  // here, we create a rectangle that represents a path with the margin on both sides.
  
  // first we have to get the normalized perpendicular vector to the path direction
  double path_length = sqrt(path.vector.x * path.vector.x + path.vector.y * path.vector.y);
  Vector dir = {path.vector.x / path_length, path.vector.y / path_length};
  Vector normal = normal_vector(dir);

  // Scale normal by margin
  Vector offset = {normal.x * margin, normal.y * margin};

  // Calculate the four corners of the corridor rectangle:
  // Starting from start_point, going to endpoint (start + vector)
  // Offset by margin in the normal direction on both sides
  Rectangle rect;

  // Corner 0: start - offset
  rect.corner[0] = (Vector){
    path.start_point.x - offset.x,
    path.start_point.y - offset.y
  };

  // Corner 1: end - offset (follow the path direction first!)
  rect.corner[1] = (Vector){
    path.start_point.x + path.vector.x - offset.x,
    path.start_point.y + path.vector.y - offset.y
  };

  // Corner 2: end + offset
  rect.corner[2] = (Vector){
    path.start_point.x + path.vector.x + offset.x,
    path.start_point.y + path.vector.y + offset.y
  };

  // Corner 3: start + offset
  rect.corner[3] = (Vector){
    path.start_point.x + offset.x,
    path.start_point.y + offset.y
  };
  
  return rect;
}

// helper function to check if all spaces are accessible from at least one path
int spaces_accessible(const Lot lot, double max_distance) {
  // for each space, check if it overlaps with at least one path's accessibility corridor
  for (int i = 0; i < lot.space_count; i++) {
    Rectangle space_rect = get_space_rectangle(lot.spaces[i]);
    int accessible = 0;

    // check against each path
    for (int j = 0; j < lot.path_count; j++) {
      // only check paths on the same level as the space
      if (lot.spaces[i].location.level != lot.paths[j].start_point.level) { continue; }
      
      Rectangle path_corridor = get_path_corridor(lot.paths[j], max_distance);
      
      // if no separating axis found → space overlaps with corridor → accessible
      if (!separating_axis(path_corridor, space_rect)) {
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

// helper function to check if entrance and POI are on valid levels
int has_valid_entrance_and_poi(const Lot lot) {
  // Check entrance is on a valid level
  if (lot.entrance.level < 0 || lot.entrance.level >= lot.level_count) {
    return 0;
  }

  // Check POI is on a valid level
  if (lot.POI.level < 0 || lot.POI.level >= lot.level_count) {
    return 0;
  }

  return 1;
}

// helper function to check if all space names are unique
int spaces_have_unique_names(const Lot lot) {
  for (int i = 0; i < lot.space_count; i++) {
    for (int j = i + 1; j < lot.space_count; j++) {
      // compare names using strcmp
      if (strcmp(lot.spaces[i].name, lot.spaces[j].name) == 0) {
        return 0; // duplicate name found
      }
    }
  }
  return 1; // all names are unique
}

// helper function to check if the number of ups and downs is correct for the level count
int has_correct_up_down_count(const Lot lot) {
  if (lot.level_count <= 1) {
    return 1; // single level, no ups/downs needed
  }
  int required = lot.level_count - 1;
  return (lot.up_count == required) && (lot.down_count == required);
}

// helper function to check if each level has appropriate ups and downs
int levels_have_ups_and_downs(const Lot lot) {
  if (lot.level_count <= 1) {
    return 1; // single level, no ups/downs needed
  }
  
  int *has_up = calloc(lot.level_count, sizeof(int));
  int *has_down = calloc(lot.level_count, sizeof(int)); // calloc simply initializes every entry to 0 (false)
  
  // step 1: mark which levels have ups and downs
  for (int i = 0; i < lot.up_count; i++) {
    int level = lot.ups[i].level; // looping over every up's level
    if (level >= 0 && level < lot.level_count) {
      has_up[level] = 1;
    }
  }
  for (int i = 0; i < lot.down_count; i++) {
    int level = lot.downs[i].level; // looping over every down's level
    if (level >= 0 && level < lot.level_count) {
      has_down[level] = 1;
    }
  }

  // step 2: verify the levels have the required ups and downs
  for (int i = 0; i < lot.level_count - 1; i++) {
    // every level except the top must have an up
    if (!has_up[i]) {
      free(has_up);
      free(has_down);
      return 0;
    }
  }
  
  for (int i = 1; i < lot.level_count; i++) {
    // every level except the bottom must have a down
    if (!has_down[i]) {
      free(has_up);
      free(has_down);
      return 0;
    }
  }

  free(has_up);
  free(has_down);
  return 1;
}
