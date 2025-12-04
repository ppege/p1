#include <stdlib.h>
#include "nav.h"
#include "calculations.h"
#include "validate.h"
#include "data.h"

Location closest_point_on_path(const Path path, const Space space) {
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

Path* available_paths(const Lot lot, const Space space, double max_distance, int* out_count) {
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
