#include "data.h"
#include "calculations.h"
#include <float.h>

Location get_endpoint(const Path *path) {
  // Calculate the endpoint of the path based on its start_point and vector
  Location endpoint;
  endpoint.x = path->start_point.x + path->vector.x;
  endpoint.y = path->start_point.y + path->vector.y;
  endpoint.level = path->start_point.level;
  return endpoint;
}

int get_occupied_space_from_car(Lot *lot, int CarIndex) {
  for (int i = 0; i < lot->space_count; i++) {
    if (lot->spaces[i].occupied == CarIndex) {
      return i; // Return the index of the occupied space
    }
  }
  return -1; // Car not found in any occupied space
}

int CheckInOrOut(Lot *lot, int CarIndex) {
  int index = get_occupied_space_from_car(lot, CarIndex);
  if (index != -1) {
    // Car is already checked in, so check it out
    lot->spaces[index].occupied = -1;
    return -1; // Checked out
  } else {
    // Car is not checked in, so check it in
    // Find the best available space
    // TODO: Space finder alg
    // For now, just find the first available space
    for (int i = 0; i < lot->space_count; i++) {
      if (lot->spaces[i].occupied == -1) {
        // Found an available space, check the car in here
        lot->spaces[i].occupied = CarIndex;
        return i; // Return the index of the space where the car was checked in
      }
    }
    // Something went wrong if we reach here
    return -2;
  }
}

Rectangle get_space_rectangle(const Space *space) {
  Dimension dim = standardized_spaces[space->type];
  double angle_rad = degrees_to_radians(space->rotation);

  // first we find the four corners relative to the bottom-left corner.
  Vector local_corners[4] = {
      {0, 0},
      {dim.width, 0},
      {dim.width, dim.height},
      {0, dim.height} // self-explanatory
  };

  Rectangle rect;
  for (int i = 0; i < 4; i++) {
    // since each "corner" is really a vector from the bottom left to corner i,
    // we can just rotate this vector.
    Vector rotated = rotate_vector(local_corners[i], angle_rad);
    // then we use simple vector math to find the vector from the origin to the
    // corner in world space, ie a coordinate.
    rect.corner[i].x = rotated.x + space->location.x;
    rect.corner[i].y = rotated.y + space->location.y;
  }

  return rect; // we now have the absolute coordinates of every corner of the
               // space given.
}
