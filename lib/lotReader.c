#include "data.h"
#include "lot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Space readSpace(char *line) {
  // Reading a space from a line
  // It consists of name, type, location(x,y,level), rotation
  char name[3];
  double x, y, rotation;
  int level, itype;

  // Getting info from the line
  sscanf(line, "name=%s type=%d location(x=%lf y=%lf level=%d) rotation=%lf",
         name, &itype, &x, &y, &level, &rotation);

  // Updating the values
  SpaceType type = (SpaceType)itype;
  Space space = {.type = type,
                 .location = {x, y, level},
                 .rotation = rotation,
                 .name = strdup(name)};

  printf("Created space: Name=%s, Type=%d, Location=(%.2f, %.2f %d),  "
         "Rotation=%.2f\n",
         space.name, space.type, space.location.x, space.location.y,
         space.location.level, space.rotation);

  return space;
};

Path readPath(char *line) {
  // Reading a path from a line
  // It consists of vector(x,y) and location(x,y,level)
  double vx, vy;
  double location_x, location_y;
  int level;

  // Getting info from the line
  sscanf(line, "vec(x=%lf y=%lf) location(x=%lf y=%lf level=%d)", &vx, &vy,
         &location_x, &location_y, &level);

  // Updating the values
  Path path = {.vector = {vx, vy},
               .start_point = {location_x, location_y, level}};

  printf("Created path: Vector=(%.2f, %.2f), location=(%.2f, %.2f, "
         "Level %d)\n",
         path.vector.x, path.vector.y, path.start_point.x, path.start_point.y,
         path.start_point.level);

  return path;
};

Location readLocation(char *line) {
  // Reading a location from a line
  double x, y;
  int level;

  // Getting info from the line
  sscanf(line, "x=%lf y=%lf level=%d", &x, &y, &level);

  // Updating the values
  Location loc = {x, y, level};

  printf("Created location: (%.2f, %.2f, Level %d)\n", loc.x, loc.y, loc.level);
  return loc;
};

void readLotFromFile(char *filename, Lot *lot) {
  // Opening the file for reading
  FILE *fptr = fopen(filename, "r");

  // Buffer to hold each line and counting variables
  char buffer[67];
  int stage = 0;
  int SpaceCount = 0;
  int PathCount = 0;
  int UpCount = 0;
  int DownCount = 0;

  // Checking if file opened successfully
  if (fptr == NULL) {
    printf("ERROR: Cant open file!\n");
    exit(1);
  }

  // Read each line from the file
  while (fgets(buffer, sizeof(buffer), fptr)) {
    if (strlen(buffer) <= 1) {
      // This is an empty line, skip it
      continue;
    }
    if (buffer[0] == '#') {
      // This is a comment line, skip it
      continue;
    }

    if (buffer[0] == '[') {
      // This is a header line
      // Determine which section we are in
      if (strcmp(buffer, "[Spaces]\n") == 0) {
        stage = 1;
      } else if (strcmp(buffer, "[Paths]\n") == 0) {
        stage = 2;
      } else if (strcmp(buffer, "[Locations]\n") ==
                 0) { // TODO: Remove since its unused
        stage = 3;
      } else if (strcmp(buffer, "[Ups]\n") == 0) {
        stage = 4;
      } else if (strcmp(buffer, "[Downs]\n") == 0) {
        stage = 5;
      } else if (strcmp(buffer, "[POI]\n") == 0) {
        stage = 6;
      } else if (strcmp(buffer, "[Entrance]\n") == 0) {
        stage = 7;
      } else {
        // Unknown header, "handle" error
        printf("Unknown header: |%s|\n", buffer);
        exit(1);
      }
      continue;
    }

    // This is a data line
    if (stage == 1) {
      // Process space data
      Space space = readSpace(buffer);
      lot->spaces[SpaceCount++] = space;
      if (SpaceCount == lot->space_count) {
        // Resize the spaces array if needed
        lot->space_count += 25;
        lot->spaces = realloc(lot->spaces, lot->space_count * sizeof(Space));
      }
    } else if (stage == 2) {
      // Process path data
      Path path = readPath(buffer);
      lot->paths[PathCount++] = path;
      if (PathCount == lot->path_count) {
        // Resize the paths array if needed
        lot->path_count += 25;
        lot->paths = realloc(lot->paths, lot->path_count * sizeof(Path));
      }
    } else if (stage > 2 && stage < 8) {
      // It has to be stage 3,4,5,6 or 7 since they are all just a location
      // Is still different stages for making it easier to read
      // Process location data
      Location location = readLocation(buffer);
      if (stage == 4) {
        lot->ups[UpCount++] = location;
        if (UpCount == lot->up_count) {
          // Resize the ups array if needed
          lot->up_count += 10;
          lot->ups = realloc(lot->ups, lot->up_count * sizeof(Location));
        }
      } else if (stage == 5) {
        lot->downs[DownCount++] = location;
        if (DownCount == lot->down_count) {
          // Resize the downs array if needed
          lot->down_count += 10;
          lot->downs = realloc(lot->downs, lot->down_count * sizeof(Location));
        }
      } else if (stage == 6) {
        // POI
        lot->POI = location;
      } else if (stage == 7) {
        // Entrance
        lot->entrance = location;
      }
    } else {
      // Unknown stage, handle error
      printf("Unknown stage: %d\n", stage);
      exit(1);
      continue;
    }
  }

  // Updating the counts in the lot
  lot->space_count = SpaceCount;
  lot->path_count = PathCount;
  lot->up_count = UpCount;
  lot->down_count = DownCount;
  // Resizing the arrays to fit the actual counts
  lot->spaces = realloc(lot->spaces, lot->space_count * sizeof(Space));
  lot->paths = realloc(lot->paths, lot->path_count * sizeof(Path));
  lot->ups = realloc(lot->ups, lot->up_count * sizeof(Location));
  lot->downs = realloc(lot->downs, lot->down_count * sizeof(Location));

  // There is only one up per level and then the ground level
  // Hence level count is up_count + 1
  lot->level_count = lot->up_count + 1;

  // Closing file again
  fclose(fptr);
}
