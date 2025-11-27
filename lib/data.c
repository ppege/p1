#include "data.h"

Location get_endpoint(const Path *path) {
  // Calculate the endpoint of the path based on its start_point and vector
  Location endpoint;
  endpoint.x = path->start_point.x + path->vector.x;
  endpoint.y = path->start_point.y + path->vector.y;
  endpoint.level = path->start_point.level;
  return endpoint;
}
