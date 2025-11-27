#include "validate.h"
#include "data.h"

int validate_lot(const Lot *lot) {
  // To validate a lot, the following rules must be checked:
  // 1. Each path must connect and each space must be accessible from at least one path.
  // 2. Spaces must not overlap.
  // 3. No obstacles (spaces) must encroach within 1.5 meters of either side of the path centerline.
  // 4. Spaces must be within 6-7 meters of a path (to be accessible)
  // 5. There must be exactly one entrance and one POI defined.
  // 6. If the amount of levels n is greater than 1, there must be n-1 ups and n-1 downs.
  // 7. Each level must have an up and a down unless they lead to non existing levels. (each level must be accessible and reversible)
  // If all rules are satisfied, return 1 (true). Otherwise, return 0 (false).
  
}

int paths_connected(Path* paths, int path_count) {
  // Implement logic to check if all paths are connected
  // each path has a start_point and a vector

}
