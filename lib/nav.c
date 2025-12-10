#include <stdlib.h>
#include "nav.h"
#include "calculations.h"
#include "validate.h"
#include "data.h"

// Helper function to find the point on a path closest to a given space
// this is essentially where the car would turn off the path to reach the space
// caution: uses fucked up vector math :c
static Location closest_point_on_path(const Path path, const Space space) {
  // Get the endpoint of the path segment
  Location endpoint = get_endpoint(path);
  
  // Convert locations to vectors for calculations
  Vector start = {path.start_point.x, path.start_point.y};
  Vector end = {endpoint.x, endpoint.y};

  // Get the space rectangle; corner 0 is the bottom left and corner 1 is the bottom right
  Rectangle space_rect = get_space_rectangle(space);
  Vector point = {
    (space_rect.corner[0].x + space_rect.corner[1].x) / 2.0,
    (space_rect.corner[0].y + space_rect.corner[1].y) / 2.0
  };
  
  // Get the segment direction vector and its squared length
  Vector segment = path.vector;
  double segment_length_sq = vector_dot_product(segment, segment);
  
  // Vector from start_point to the space's location
  Vector to_point = subtract_vectors(point, start);
  
  // Calculate projection parameter t using dot product
  // t = 0 means at start_point, t = 1 means at endpoint
  double t = vector_dot_product(to_point, segment) / segment_length_sq;
  
  // Clamp t to [0, 1] to keep within segment bounds
  if (t < 0.0) {
    t = 0.0;
  } else if (t > 1.0) {
    t = 1.0;
  }
  
  // Calculate the closest point on the segment: start + t * segment
  Vector scaled_dir = vector_scale(segment, t);
  Vector closest = vector_add(start, scaled_dir);
  
  return (Location){closest.x, closest.y, path.start_point.level};
}

// Helper function to get the subpath from path start to endpoint
// this subpath is a segment of the original path
static Path get_subpath(const Path path, const Location endpoint) {
  if (endpoint.level != path.start_point.level) {
    return path; // must be same level
  }

  // return the path from the start of path to the endpoint
  return (Path){
    .start_point = path.start_point,
    .vector = {
      endpoint.x - path.start_point.x,
      endpoint.y - path.start_point.y
    }
  };
}

// Helper function to get the turnpath from subpath endpoint to space
// useful for comparing route lengths since spaces distance from path may vary
static Path get_turnpath(const Location subpath_endpoint, const Space destination_space) {
  if (subpath_endpoint.level != destination_space.location.level) {
    return (Path){
      .start_point = subpath_endpoint,
      .vector = {0, 0}
    }; // must be same level
  }

  // get the space rectangle; corner 0 is the bottom left and corner 1 is the bottom right
  // halfway point between these will be the middle of the entry point to the space
  Rectangle space_rect = get_space_rectangle(destination_space);
  Location entry_point = {
    .x = (space_rect.corner[0].x + space_rect.corner[1].x) / 2.0,
    .y = (space_rect.corner[0].y + space_rect.corner[1].y) / 2.0,
    .level = destination_space.location.level
  };

  // return the path from the subpath endpoint to the destination space,
  // ie the small implicit path from the closest point on the path to the space, to the space itself.
  return (Path){
    .start_point = subpath_endpoint,
    .vector = {
      entry_point.x - subpath_endpoint.x,
      entry_point.y - subpath_endpoint.y
    }
  };
}

// Helper function to find all paths that can access a given space
static Path* available_paths(const Lot lot, const Space space, double max_distance, int* out_count) {
  Path* good_paths = malloc(sizeof(Path) * lot.path_count);
  int count = 0;

  // use code from validation rule 4 to see which paths can access this space
  Rectangle space_rect = get_space_rectangle(space);
  for (int i = 0; i < lot.path_count; i++) {
    if (space.location.level != lot.paths[i].start_point.level) { continue; }
    Rectangle path_corridor = get_path_corridor(lot.paths[i], max_distance);
    if (!separating_axis(path_corridor, space_rect)) {
      good_paths[count] = lot.paths[i];
      count++;
    }
  }
  *out_count = count;
  return realloc(good_paths, sizeof(Path) * count);
}

// function to calculate the total length of a superpath
double superpath_length(const Path* superpath, int count) {
  double total_length = 0.0;
  for (int i = 0; i < count; i++) {
    total_length += vector_length(superpath[i].vector);
  }
  return total_length;
}

// Helper function to find all paths that end at a given location
static Path* paths_with_endpoint(const Lot lot, const Location endpoint, int* out_count) {
  Path* matching_paths = malloc(sizeof(Path) * lot.path_count); // worst case: all paths match (ridiculous)
  int count = 0;

  for (int i = 0; i < lot.path_count; i++) {
    Location path_endpoint = get_endpoint(lot.paths[i]);
    if (compare_locations(path_endpoint, endpoint)) {
      matching_paths[count] = lot.paths[i];
      count++;
    }
  }
  *out_count = count;
  return realloc(matching_paths, sizeof(Path) * count);
}

// Check if a location is an "up" location on its level
static int is_up_location(const Lot lot, const Location loc) {
  for (int i = 0; i < lot.up_count; i++) {
    if (compare_locations(lot.ups[i], loc)) {
      return 1;
    }
  }
  return 0;
}

// Check if a location is a "down" location on its level
static int is_down_location(const Lot lot, const Location loc) {
  for (int i = 0; i < lot.down_count; i++) {
    if (compare_locations(lot.downs[i], loc)) {
      return 1;
    }
  }
  return 0;
}

// Get the "up" location on a given level (NULL if none exists)
static Location* get_up_on_level(const Lot lot, int level) {
  for (int i = 0; i < lot.up_count; i++) {
    if (lot.ups[i].level == level) {
      return &lot.ups[i];
    }
  }
  return NULL;
}

// Get the "down" location on a given level (NULL if none exists)
static Location* get_down_on_level(const Lot lot, int level) {
  for (int i = 0; i < lot.down_count; i++) {
    if (lot.downs[i].level == level) {
      return &lot.downs[i];
    }
  }
  return NULL;
}

// Trace paths backwards from start until we hit the entrance
// Recursively traverses levels via up/down locations
static Path* follow_to_entrance(const Lot lot, const Location start, int* out_count) {
  // Base case: we've reached the entrance
  if (compare_locations(start, lot.entrance)) {
    *out_count = 0;
    return NULL;
  }

  // Find all paths that end at our current location on the same level
  int count = 0;
  Path* candidates = paths_with_endpoint(lot, start, &count);

  int same_level_count = 0;
  for (int i = 0; i < count; i++) {
    if (candidates[i].start_point.level == start.level) {
      candidates[same_level_count++] = candidates[i];
    }
  }

  // Dead end on this level - check if we can traverse to another level
  if (same_level_count == 0) {
    free(candidates);

    // At an "up" location - we must GO UP to get back to entrance
    // Continue from the "down" on the level above
    if (is_up_location(lot, start)) {
      Location* above = get_down_on_level(lot, start.level + 1);
      if (above) {
        return follow_to_entrance(lot, *above, out_count);
      }
    }

    // At a "down" location - we must GO DOWN to get back to entrance
    // Continue from the "up" on the level below
    if (is_down_location(lot, start)) {
      Location* below = get_up_on_level(lot, start.level - 1);
      if (below) {
        return follow_to_entrance(lot, *below, out_count);
      }
    }

    // Dead end and can't traverse levels - no valid path
    *out_count = -1;
    return NULL;
  }

  // Track the best (shortest) route found so far
  Path* best_result = NULL;
  int best_count = -1;
  double best_length = -1.0;

  for (int i = 0; i < same_level_count; i++) {
    Location new_start = candidates[i].start_point;
    int sub_count = 0;
    Path* sub_path = follow_to_entrance(lot, new_start, &sub_count);

    if (sub_count >= 0) {
      int total_count = sub_count + 1;

      Path* candidate_result = malloc(sizeof(Path) * total_count);
      for (int j = 0; j < sub_count; j++) {
        candidate_result[j] = sub_path[j];
      }
      candidate_result[sub_count] = candidates[i];

      double candidate_length = superpath_length(candidate_result, total_count);

      if (best_length < 0 || candidate_length < best_length) {
        free(best_result);
        best_result = candidate_result;
        best_count = total_count;
        best_length = candidate_length;
      } else {
        free(candidate_result);
      }

      free(sub_path);
    }
  }

  free(candidates);

  *out_count = best_count;
  return best_result;
}

// Main function to find the best superpath from lot entrance to a space
Path* superpath_to_space(const Lot lot, const Space space, int* out_count) {
  // first we need to find all paths that can access this space
  int count = 0;
  Path* available = available_paths(lot, space, path_accessibility, &count);

  // now we need to evaluate each available path to find the best superpath
  double best_length = -1.0;
  Path* best_superpath = NULL;

  for (int i = 0; i < count; i++) {
    // first we get the relevant paths
    Location closest = closest_point_on_path(available[i], space);
    Path subpath = get_subpath(available[i], closest);
    Path turnpath = get_turnpath(closest, space);

    // then to calculate the superpath we just follow the start points
    int super_count = 0;
    Path* superpath = follow_to_entrance(lot, subpath.start_point, &super_count);

    // handle the case where no valid superpath was found
    if (super_count == -1) {
      continue;
    }

    // build the full path where we just add the subpath and turnpath
    Path* full_path = malloc(sizeof(Path) * (super_count + 2));
    for (int j = 0; j < super_count; j++) {
      full_path[j] = superpath[j]; // copying the results of follow_to_entrance over to the larger full_path
    }
    free(superpath); // we can free this now as we've copied its contents

    full_path[super_count] = subpath;
    full_path[super_count + 1] = turnpath;

    // we then wanna see if this is the best path so far
    double full_length = superpath_length(full_path, super_count + 2);
    if (best_length < 0 || full_length < best_length) {
      // this is either the best or the first valid path we've found
      free(best_superpath); // free previous best (yes it's STILL safe to free NULL)
      best_superpath = full_path;
      best_length = full_length;
      *out_count = super_count + 2;
    } else {
      free(full_path); // not the best, so free it
    }
  }

  // after all this, we have the best superpath (or NULL if none found)
  free(available);
  return best_superpath;
}
