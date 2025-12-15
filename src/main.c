#include "PlateDB.h"
#include "data.h"
#include "display.h"
#include "image.h"
#include "lot.h"
#include "lotReader.h"
#include "nav.h"
#include "stdlib.h"
#include "validate.h"
#include <stdio.h>

int main() {
  char *LotFileName = "example.lot";
  Lot lot = lot_from_file(LotFileName);

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

    int box_width = 67;

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

    int index = get_occupied_space_from_car(&lot, Res);
    if (index != -1) {
      // Car is already checked in, so check it out
      lot.spaces[index].occupied = -1;
      printf("Car with plate %s checked out successfully.\n", TempPlate);
      printf("Thank you for using our parking lot! Goodbye!\n");
      continue;
    }
    // Car is not checked in, so check it in
    // Find the best available space
    Space *foundSpace = best_space(lot, CarArr[index].carType);
    if (foundSpace == NULL) {
      printf("No available space found for car with plate %s.\n", TempPlate);
      if (CarArr[index].carType != Standard) {
        printf("This may be due to the car type (%s) not having any "
               "available spaces.\n");
        printf("Would you line to try checking in as a standard car? (y/n): ");
        char response = getchar();
        getchar(); // consume newline
        // Clear clear_screen
        if (response == 'y' || response == 'Y') {
          foundSpace = best_space(lot, Standard);
          if (foundSpace->name == NULL) {
            printf("No available standard space found for car with plate %s.\n",
                   TempPlate);
            continue;
          } else {
            foundSpace->occupied = Res;
            printf("Car with plate %s checked in successfully to space %s as "
                   "a standard car.\n",
                   TempPlate, foundSpace->name);
          }
          continue;
        }
        foundSpace->occupied = Res;
        printf("Car with plate %s checked in successfully to space %s.\n",
               TempPlate, foundSpace->name);
      }
      continue;
    } else {
      foundSpace->occupied = Res;
      printf("Car with plate %s checked in successfully to space %s.\n",
             TempPlate, foundSpace->name);
    }
    // Print navigation instructions
    int lenght = 0;
    Path *superpath = superpath_to_space(lot, *foundSpace, &lenght);
    if (superpath == NULL || lenght <= 0) {
      printf("No navigation path found to space %s.\n", foundSpace->name);
      continue;
    }
    lot_to_ppm_all_levels(lot, "outImg", 10, superpath, lenght);
    printf(
        "Navigation path to space %s generated and saved as outshit_LvX.ppm.\n",
        foundSpace->name);
  }
  free(CarArr);
  return 0;
}
