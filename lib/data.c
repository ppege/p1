#include "data.h"
#include "stdio.h"
#include "string.h"

Location get_endpoint(const Path *path) {
  // Calculate the endpoint of the path based on its start_point and vector
  Location endpoint;
  endpoint.x = path->start_point.x + path->vector.x;
  endpoint.y = path->start_point.y + path->vector.y;
  endpoint.level = path->start_point.level;
  return endpoint;
}

int GetOccupancyIndexFromSpace(Occupancy *occupancies, int occupancy_count,
                               const char *Space_Id) {
  // Search for the occupancy with the given Space_Id
  for (int i = 0; i < occupancy_count; i++) {
    // Avoiding dereferencing NULL pointers
    // Heard that was bad 👎
    if (occupancies[i].Space_Id == NULL) {
      continue; // Empty spot found
    }
    if (strcmp(occupancies[i].Space_Id, Space_Id) == 0) {
      return i; // Found the occupancy, return its index
    }
  }
  return -1; // Not found
}

int GetOccupancyIndexFromCar(Occupancy *occupancies, int occupancy_count,
                             int CarIndex) {
  // Search for the occupancy with the given CarIndex
  for (int i = 0; i < occupancy_count; i++) {
    if (occupancies[i].CarIndex == CarIndex) {
      return i; // Found the occupancy, return its index
    }
  }
  return -1; // Not found
}

int CheckInOrOut(Occupancy *occupancies, int occupancy_count, int CarIndex) {
  int index = GetOccupancyIndexFromCar(occupancies, occupancy_count, CarIndex);
  if (index != -1) {
    // Car is already checked in, so check it out
    occupancies[index].CarIndex = 0;
    occupancies[index].Space_Id = NULL;
    return -1; // Checked out
  } else {
    // Car is not checked in, so check it in
    // Find first empty spot in the occupancies list
    // Put the carindex in there and return the occupancy index
    for (int i = 0; i < occupancy_count; i++) {
      if (occupancies[i].CarIndex == 0) {
        occupancies[i].CarIndex = CarIndex;
        return i; // Checked in
      }
    }
    // Something went wrong if we reach here
    return -2;
  }
}
