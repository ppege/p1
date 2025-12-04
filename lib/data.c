#include <stdio.h>
#include <float.h>
#include <math.h>
#include "data.h"
#include "calculations.h"

Location get_endpoint(const Path path) {
  // Calculate the endpoint of the path based on its start_point and vector
  Location endpoint;
  endpoint.x = path.start_point.x + path.vector.x;
  endpoint.y = path.start_point.y + path.vector.y;
  endpoint.level = path.start_point.level;
  return endpoint;
}

Rectangle get_space_rectangle(const Space space) {
  Dimension dim = standardized_spaces[space.type];
  double angle_rad = degrees_to_radians(space.rotation);
  
  // first we find the four corners relative to the bottom-left corner.
  Vector local_corners[4] = {
    {0, 0},
    {dim.width, 0},
    {dim.width, dim.height},
    {0, dim.height} // self-explanatory
  };

  Rectangle rect;
  for (int i = 0; i < 4; i++) {
    // since each "corner" is really a vector from the bottom left to corner i, we can just rotate this vector.
    Vector rotated = rotate_vector(local_corners[i], angle_rad);
    // then we use simple vector math to find the vector from the origin to the corner in world space, ie a coordinate.
    rect.corner[i].x = rotated.x + space.location.x;
    rect.corner[i].y = rotated.y + space.location.y;
  }

  return rect; // we now have the absolute coordinates of every corner of the space given.
}


