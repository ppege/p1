#include "data.h"
#include "lot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Space* readSpace(const char *line) {
  // Reading a space from a line
  // It consists of name, type, location(x,y,level), rotation
  char name[11];
  double x, y, rotation;
  int level, itype;

  // Getting info from the line
  if (!line) return NULL; // dont dereference a null pointer dude
  int n = sscanf(line, "name=%10s type=%d location(x=%lf y=%lf level=%d) rotation=%lf",
         name, &itype, &x, &y, &level, &rotation);

  // Validating the read
  if (n != 6) {
    printf("ERROR: Invalid space line format: %s\n", line);
    return NULL;
  }

  // Updating the values
  SpaceType type = (SpaceType)itype;

  // create the space and return pointer to it
  Space* space = malloc(sizeof(Space));
  if (!space) return NULL; // malloc failed
  space->type = type;
  space->location = (Location){x, y, level};
  space->rotation = rotation;
  space->name = strdup(name);
  space->occupied = -1;

  if (!space->name) {
    free(space);
    return NULL; // strdup can fail
  }

  // printf("Created space: Name=%s, Type=%d, Location=(%.2f, %.2f %d),  "
  //        "Rotation=%.2f\n",
  //        space.name, space.type, space.location.x, space.location.y,
  //        space.location.level, space.rotation);

  return space;
};

Path* readPath(const char *line) {
  // Reading a path from a line
  // It consists of vector(x,y) and location(x,y,level)
  double vx, vy;
  double location_x, location_y;
  int level;

  // Getting info from the line
  if (!line) return NULL; // dont dereference a null pointer dude
  int n = sscanf(line, "vec(x=%lf y=%lf) location(x=%lf y=%lf level=%d)", &vx, &vy,
         &location_x, &location_y, &level);

  // Validating the read
  if (n != 5) {
    printf("ERROR: Invalid path line format: %s\n", line);
    return NULL;
  }

  // create a path
  Path* path = malloc(sizeof(Path));
  if (!path) return NULL; // malloc failed

  // Updating the values
  path->vector = (Vector){vx, vy};
  path->start_point = (Location){location_x, location_y, level};

  // printf("Created path: Vector=(%.2f, %.2f), location=(%.2f, %.2f, "
  //        "Level %d)\n",
  //        path.vector.x, path.vector.y, path.start_point.x,
  //        path.start_point.y, path.start_point.level);
  return path;
};

Location* readLocation(const char *line) {
  // Reading a location from a line
  double x, y;
  int level;

  // Getting info from the line
  if (!line) return NULL; // dont dereference a null pointer dude
  int n = sscanf(line, "x=%lf y=%lf level=%d", &x, &y, &level);

  // Validating the read
  if (n != 3) {
    printf("ERROR: Invalid location line format: %s\n", line);
    return NULL;
  }

  // create location
  Location* loc = malloc(sizeof(Location));
  if (!loc) return NULL; // malloc failed

  // Updating the values
  *loc = (Location){x, y, level};

  // printf("Created location: (%.2f, %.2f, Level %d)\n", loc.x, loc.y,
  // loc.level);
  return loc;
};

// taking a filename, read the lot data from the file and return a Lot struct
Lot lot_from_file(char *filename) {
  // initialize a lot struct with 1 of everything
  Lot lot = create_lot(1, 1, 1, 1, 1);
  lot.ramp_length = 40.0; // default ramp length
  // Opening the file for reading
  FILE *fptr = fopen(filename, "r");

  // Buffer to hold each line and counting variables
  char buffer[420];
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
      } else if (strcmp(buffer, "[Ups]\n") == 0) {
        stage = 3;
      } else if (strcmp(buffer, "[Downs]\n") == 0) {
        stage = 4;
      } else if (strcmp(buffer, "[POI]\n") == 0) {
        stage = 5;
      } else if (strcmp(buffer, "[Entrance]\n") == 0) {
        stage = 6;
      } else if (strcmp(buffer, "[Ramp Length]\n") == 0) {
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
      Space *space = readSpace(buffer);
      if (!space) {
        printf("ERROR: Failed to read space from line: %s\n", buffer);
        exit(1);
      }
      lot.spaces[SpaceCount++] = *space;
      free(space->name);
      free(space);
      if (SpaceCount == lot.space_count) {
        // Resize the spaces array if needed
        lot.space_count += 25;
        Space* spaceres = realloc(lot.spaces, lot.space_count * sizeof(Space));
        if (!spaceres) {
          printf("ERROR: Memory reallocation failed!\n");
          exit(1);
        }
        lot.spaces = spaceres;
      }
    } else if (stage == 2) {
      // Process path data
      Path *path = readPath(buffer);
      if (!path) {
        printf("ERROR: Failed to read path from line: %s\n", buffer);
        exit(1);
      }
      lot.paths[PathCount++] = *path;
      free(path);
      if (PathCount == lot.path_count) {
        // Resize the paths array if needed
        lot.path_count += 25;
        Path* pathres = realloc(lot.paths, lot.path_count * sizeof(Path));
        if (!pathres) {
          printf("ERROR: Memory reallocation failed!\n");
          exit(1);
        }
        lot.paths = pathres;
      }
    } else if (stage > 2 && stage < 7) {
      // It has to be stage 3,4,5, or 6 since they are all just a location
      // Is still different stages for making it easier to read
      // Process location data
      Location *location = readLocation(buffer);
      if (!location) {
        printf("ERROR: Failed to read location from line: %s\n", buffer);
        exit(1);
      }
      if (stage == 3) {
        lot.ups[UpCount++] = *location;
        if (UpCount == lot.up_count) {
          // Resize the ups array if needed
          lot.up_count += 10;
          Up* upres = realloc(lot.ups, lot.up_count * sizeof(Location));
          if (!upres) {
            printf("ERROR: Memory reallocation failed!\n");
            exit(1);
          }
          lot.ups = upres;
        }
      } else if (stage == 4) {
        lot.downs[DownCount++] = *location;
        if (DownCount == lot.down_count) {
          // Resize the downs array if needed
          lot.down_count += 10;
          Down* downres = realloc(lot.downs, lot.down_count * sizeof(Location));
          if (!downres) {
            printf("ERROR: Memory reallocation failed!\n");
            exit(1);
          }
          lot.downs = downres;
        }
      } else if (stage == 5) {
        // POI
        lot.POI = *location;
      } else if (stage == 6) {
        // Entrance
        lot.entrance = *location;
      }
      free(location);
    } else if (stage == 7) {
      double length = 0.0;
      int n = sscanf(buffer, "%lf", &length);
      if (n != 1) {
        printf("ERROR: Invalid ramp length line format: %s\n", buffer);
        exit(1);
      }
      lot.ramp_length = length;
    } else {
      // Unknown stage, handle error
      printf("Unknown stage: %d\n", stage);
      exit(1);
    }
  }

  // Updating the counts in the lot
  lot.space_count = SpaceCount;
  lot.path_count = PathCount;
  lot.up_count = UpCount;
  lot.down_count = DownCount;
  // Resizing the arrays to fit the actual counts
  Space* spaceres = realloc(lot.spaces, lot.space_count * sizeof(Space));
  Path* pathres = realloc(lot.paths, lot.path_count * sizeof(Path));
  Up* upres = realloc(lot.ups, lot.up_count * sizeof(Location));
  Down* downres = realloc(lot.downs, lot.down_count * sizeof(Location));

  if (!spaceres || !pathres || !upres || !downres) {
    printf("ERROR: Memory reallocation failed!\n");
    exit(1);
  }

  lot.spaces = spaceres;
  lot.paths = pathres;
  lot.ups = upres;
  lot.downs = downres;

  // we call this ridiculous helper function to count the unique levels in the
  // lot
  lot.level_count = count_levels(lot);

  // Closing file again
  fclose(fptr);
  return lot;
}
