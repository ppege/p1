#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Space readSpace(char *line) {

};

Path readPath(char *line) {

};

Location readLocation(char *line) {

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
  int amount = 0;
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
      printf("Amount is %d\n", amount);
      if (amount > 0) {
        // This is a data line, process it
        // e.g., space, path, location data
        printf("This is a data line\n");
        if (stage == 1) {
          // Process space data
          Space space = readSpace(buffer);
          // Add space to lot structure (not implemented here)
          printf("Processed space data\n");
        } else if (stage == 2) {
          // Process path data
          Path path = readPath(buffer);
          // Add path to lot structure (not implemented here)
          printf("Processed path data\n");
        } else if (stage == 3) {
          // Process location data
          Location location = readLocation(buffer);
          // Add location to lot structure (not implemented here)
          printf("Processed location data\n");
        } else {
          // Unknown stage, handle error
          printf("Unknown stage: %d\n", stage);
          exit(1);
        }
        amount--;
        continue;
      }
      if (buffer[0] == '[') {
        // This is a header line, process it
        // e.g., [Spaces], [Paths], [Locations], etc.
        if (strcmp(buffer, "[Spaces xxx]\n") <= 3) {
          // Process spaces
          sscanf(buffer, "[Spaces %d]\n", &amount);
          stage = 1;
          printf("This is the spaces header with %d spaces\n", amount);

        } else if (strcmp(buffer, "[Paths xxx]\n") <= 3) {
          // Process paths
          sscanf(buffer, "[Paths %d]\n", &amount);
          stage = 2;
          printf("This is the paths header\n");
        } else if (strcmp(buffer, "[Locations xxx]\n") <= 3) {
          // Process locations
          sscanf(buffer, "[Locations %d]\n", &amount);
          stage = 3;
          printf("This is the locations header\n");
        } else {
          // Unknown header, handle error
          printf("Unknown header: |%s|\n", buffer);
          exit(1);
        }
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
