// hello!
#include "PlateDB.h"
#include "data.h"
#include "stdlib.h"
#include <stdio.h>
#include <string.h>

// A function to get the number of lines in a file
int GetFileLines(char *FileName) {
  // Opening file from name
  FILE *fptr = fopen(FileName, "r");
  char buffer[11];
  int i = 0;
  // Check if the file was opened
  if (fptr != NULL) {
    // Read each line from the file
    while (fgets(buffer, sizeof(buffer), fptr)) {
      i++;
    }
  } else {
    printf("ERROR: Cant open file %s!\n", FileName);
    exit(1);
  }
  // Closing file again
  fclose(fptr);
  return i;
}

// A function to read the file into an array of Car structs
void ReadFile(Car *CarArr, int lines, char *FileName) {
  // Opens file from name
  FILE *Tempptr = fopen(FileName, "r");

  // Temp buffer
  char buffer[11];

  // Loop though the file line for line
  for (int i = 0; i < lines; i++) {
    // Read single line to buffer
    fgets(buffer, sizeof(buffer), Tempptr);
    // Setting the index of car array to the struct output from the line
    CarArr[i] = ReadLine(buffer);
  }
  // Closing file again
  fclose(Tempptr);
}

// A function to read a line into a Car struct
Car ReadLine(char line[10]) {
  char plate[8];
  int num;

  // Reading line to plate and num (for SpaceType)
  sscanf(line, " %s %d", plate, &num);

  // Creating an empty struct
  Car car;

  // Casting from int to SpaceType and adding it to the struct
  car.type = (SpaceType)num;

  // String copy into the struct
  strcpy(car.plate, plate);
  return car;
}

// A function to find the index of a given plate in the a Car array
int GetCarIndexFromPlate(Car *CarArr, int size, char plate[8]) {
  for (int i = 0; i < size; i++) {
    // Using strcmp to check if its eaqual
    if (!strcmp(CarArr[i].plate, plate)) {
      // printf("Search plate found at: %d \n", i);
      // returning the index
      return i;
    }
  }
  // retuning -1 if plate is not found
  return -1;
}
