#include <stdlib.h>
#include "nav.h"
#include "calculations.h"
#include "validate.h"
#include "data.h"

static Location closest_point_on_path(const Path path, const Space space) {
  // Get the endpoint of the path segment
  Location endpoint = get_endpoint(path);
  
  // Convert locations to vectors for calculations
  Vector start = {path.start_point.x, path.start_point.y};
  Vector end = {endpoint.x, endpoint.y};
  Vector point = {space.location.x, space.location.y};
  
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

static Path get_turnpath(const Location subpath_endpoint, const Space destination_space) {
  if (subpath_endpoint.level != destination_space.location.level) {
    return (Path){
      .start_point = subpath_endpoint,
      .vector = {0, 0}
    }; // must be same level
  }

  // return the path from the subpath endpoint to the destination space,
  // ie the small implicit path from the closest point on the path to the space, to the space itself.
  return (Path){
    .start_point = subpath_endpoint,
    .vector = {
      destination_space.location.x - subpath_endpoint.x,
      destination_space.location.y - subpath_endpoint.y
    }
  };
}

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

static double superpath_length(const Path* superpath, int count) {
  double total_length = 0.0;
  for (int i = 0; i < count; i++) {
    total_length += vector_length(superpath[i].vector);
  }
  return total_length;
}

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

static Path* follow_to_entrance(const Lot lot, const Location start, int* out_count) {
  // Base case: if we're already at the entrance, return empty path
  // out_count = 0 indicates success with no more paths needed
  if (compare_locations(start, lot.entrance)) {
    *out_count = 0;
    return NULL;
  }

  // Find all paths that end at our current location
  // These are candidates we could have taken to get here
  int count = 0;
  Path* candidates = paths_with_endpoint(lot, start, &count);

  // Track the best (shortest) route found so far
  Path* best_result = NULL;
  int best_count = -1;       // -1 indicates no valid path found yet
  double best_length = -1.0; // -1 indicates no length measured yet

  // Try each candidate path and recursively find the best route to entrance
  for (int i = 0; i < count; i++) {
    // The start point of this candidate becomes our new target
    // (we're tracing backwards from destination to entrance)
    Location new_start = candidates[i].start_point;
    int sub_count = 0;
    Path* sub_path = follow_to_entrance(lot, new_start, &sub_count);

    // sub_count >= 0 means a valid route to the entrance was found
    if (sub_count >= 0) {
      // Build the complete superpath: sub_path + current candidate
      // Total segments = segments from entrance to new_start + 1 (current path)
      int total_count = sub_count + 1;
      
      Path* candidate_result = malloc(sizeof(Path) * total_count);
      
      // Copy the path from entrance to new_start
      for (int j = 0; j < sub_count; j++) {
        candidate_result[j] = sub_path[j];
      }
      
      // Append the current candidate (new_start to start)
      candidate_result[sub_count] = candidates[i];
      
      // Calculate the total length of the superpath
      double candidate_length = superpath_length(candidate_result, total_count);
      
      // Update best if this is the first valid route or shorter than current best
      if (best_length < 0 || candidate_length < best_length) {
        free(best_result); // Free previous best (safe to free NULL)
        best_result = candidate_result;
        best_count = total_count;
        best_length = candidate_length;
      } else {
        // This route is longer, so there's no use for it!
        free(candidate_result);
      }
      
      // Clean up the sub_path from recursion
      free(sub_path);
    }
  }

  // Clean up the candidates array
  free(candidates);
  
  // Return the best route found (or NULL with count=-1 if none found)
  *out_count = best_count;
  return best_result;
}

Path* superpath_to_space(const Lot lot, const Space space, int* out_count) {
  int count = 0;
  Path* available = available_paths(lot, space, path_accessibility, &count);

  double best_length = -1.0;
  Path* best_superpath = NULL;

  // since there can be multiple available paths, we need to check each one
  for (int i = 0; i < count; i++) {
    // first we get the relevant paths
    Location closest = closest_point_on_path(available[i], space);
    Path subpath = get_subpath(available[i], closest);
    Path turnpath = get_turnpath(closest, space);

    // then to calculate the superpath we just follow the start points
    int super_count = 0;
    Path* superpath = follow_to_entrance(lot, subpath.start_point, &super_count);
    if (super_count >= 0) {
      // we found a valid superpath from the entrance, so build the full path where we just add the subpath and turnpath
      Path* full_path = malloc(sizeof(Path) * (super_count + 2));
      for (int j = 0; j < super_count; j++) {
        full_path[j] = superpath[j];
      }
      full_path[super_count] = subpath;
      full_path[super_count + 1] = turnpath;
      // we then wanna see if this is the best path so far
      double full_length = superpath_length(full_path, super_count + 2);
      if (best_length < 0 || full_length < best_length) {
        free(best_superpath); // free previous best (safe to free NULL)
        best_superpath = full_path;
        best_length = full_length;
        *out_count = super_count + 2;
      } else {
        free(full_path); // not the best, so free it
      }
    }
    free(superpath); // clean up the superpath from follow_to_entrance
  }
  free(available);
  return best_superpath;
}
