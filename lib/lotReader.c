#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Space readSpace(char *line) {
  char name[1];
  double x, y, rotation;
  int level, itype;
  sscanf(line, "name=%s type=%d location(x=%lf y=%lf level=%d) rotation=%lf",
         name, &itype, &x, &y, &level, &rotation);

  SpaceType type = (SpaceType)itype;
  printf("test\n");
  printf("Read space: Name=%s, Type=%d, Location=(%.2f, %.2f %d),  "
         "Rotation=%.2f\n",
         name, type, x, y, level, rotation);
  printf("test\n");
  Space space = {.type = type, .location = {x, y, level}, .rotation = rotation};
  printf("test\n");
  strcpy(space.name, name);
  printf("test\n");
  // return space;
};

Path readPath(char *line) {
  double vx, vy;
  double sx, sy;
  int level;
  sscanf(line, "vec(x=%lf y=%lf) start_point(x=%lf y=%lf level=%d)", &vx, &vy,
         &sx, &sy, &level);
  Path path = {.vector = {vx, vy}, .start_point = {sx, sy, level}};
  return path;
};

Location readLocation(char *line) {
  double x, y;
  int level;
  sscanf(line, "x=%lf y=%lf level=%d", &x, &y, &level);
  Location loc = {x, y, level};
  return loc;
};

Lot readLotFromFile(char *filename) {
  // Open the filename
  // Read the data line by line
  // loop until end of file {
  //    Check headers to determine what data is being read
  //    Parse the line accordingly using readSpace, readPath, readLocation
  //    Populate the Lot structure with the parsed data
  // }
  // Close the file
  // Return the populated Lot structure
  // Opening file from name
  FILE *fptr = fopen(filename, "r");
  char buffer[67];
  int stage = 0;
  // Check if the file was opened
  //
  // Make it so it can handle an unknown amount of spaces, paths, locations
  // read the header, filter out comments and empty lines
  // Then check for new header
  // if not a header, must be data line, read the line
  if (fptr != NULL) {
    // Read each line from the file
    while (fgets(buffer, sizeof(buffer), fptr)) {
      if (strlen(buffer) <= 1) {
        // This is an empty line, skip it
        printf("This is an empty line\n");
        continue;
      }
      if (buffer[0] == '#') {
        // This is a comment line, skip it
        printf("This is a comment line\n");
        continue;
      }

      if (buffer[0] == '[') {
        // This is a header line, process it
        // e.g., [Spaces], [Paths], [Locations], etc.
        if (strcmp(buffer, "[Spaces]\n") == 0) {
          // Process spaces
          stage = 1;
          printf("This is the spaces header\n");

        } else if (strcmp(buffer, "[Paths]\n") == 0) {
          // Process paths
          stage = 2;
          printf("This is the paths header\n");
        } else if (strcmp(buffer, "[Locations]\n") == 0) {
          // Process locations
          stage = 3;
          printf("This is the locations header\n");
        } else {
          // Unknown header, handle error
          printf("Unknown header: |%s|\n", buffer);
          exit(1);
        }
        continue;
      }

      // This is a data line, process it
      // e.g., space, path, location data
      printf("This is a data line\n");
      if (stage == 1) {
        // Process space data
        printf("Pre read space\n");
        Space space = readSpace(buffer);
        // Add space to lot structure (not implemented here)
        printf("Processed space data\n");
      } else if (stage == 2) {
        // Process path data
        Path path = readPath(buffer);
        printf(
            "Path: Vector (%.2f, %.2f), Start Point (%.2f, %.2f, Level %d)\n",
            path.vector.x, path.vector.y, path.start_point.x,
            path.start_point.y, path.start_point.level);
        // Add path to lot structure (not implemented here)
        printf("Processed path data\n");
      } else if (stage == 3) {
        // Process location data
        Location location = readLocation(buffer);
        printf("Location: (%.2f, %.2f, Level %d)\n", location.x, location.y,
               location.level);
        // Add location to lot structure (not implemented here)
        printf("Processed location data\n");
      } else {
        // Unknown stage, handle error
        printf("Unknown stage: %d\n", stage);
        exit(1);
        continue;
      }
      printf("Line: |%s|", buffer);
    }
  } else {
    printf("ERROR: Cant open file!\n");
    exit(1);
  }
  // Closing file again
  fclose(fptr);
  // return i;
}
