#include "PlateDB.h"
#include "data.h"
#include "display.h"
#include "lot.h"
#include "lotReader.h"
#include "validate.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  // Create lot and read .lot file
  Lot *lot = create_lot(0, 10, 10, 10, 10);
  char *LotFileName = "test.lot";
  readLotFromFile(LotFileName, lot);

  // Validate lot
  ValidationResult result = validate_lot(lot);
  if (result.error != NoError) {
    printf("Lot validation failed with error: %s\n",
           validation_error_message(result.error));
    return 1;
  }

  // Create plateDB and read it from file
  char *PlateDBFileName = "test/test.txt";
  int lines = GetFileLines(PlateDBFileName);
  car *CarArr = (car *)malloc(sizeof(car) * lines);

  ReadFile(CarArr, lines, PlateDBFileName);

  while (1) {
    // Get a quick overview of the lot
    // print_lot(lot);

    clear_screen();

    int box_width = 40;

    box_start(box_width);
    box_line("Welcome to the parking lot!", box_width);
    box_break(box_width);
    box_line("Here we have a total of 69 parking spaces!", box_width);
    box_end(box_width);

    char TempPlate[8];
    if (scan_plate(TempPlate)) {
      printf("Plate is not valid. Please try again with a valid plate.\n");
      continue;
    }
    int Res = GetCarIndexFromPlate(CarArr, lines, TempPlate);
    if (Res == -1) {
      printf("Car with plate %s not found in database.\n", TempPlate);
      continue;
    }
    printf("Car with plate %s found in database.\n", TempPlate);

    // Check the car in/out
    // If the car is checked in, check it out and then continue
    // If the car is not checked in, check it in
    //  Then display the parking lot and find a space for the car
    //  Display the navigation to the space
    // End by saying goodbye to the user
  }

  free(CarArr);
  return 0;
}
