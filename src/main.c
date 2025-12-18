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

void sleep_ms(unsigned int milliseconds) {
#ifdef _WIN32
    #include <windows.h>
    Sleep(milliseconds);
#else
    #include <time.h>
    struct timespec ts;
    ts.tv_sec  = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000L; // L means we are using the long integer type
    nanosleep(&ts, NULL);
#endif
}

int main() {
  char *LotFileName = "parkinglot.lot";
  Lot lot = lot_from_file(LotFileName);

  // Validate lot
  ValidationResult result = validate_lot(lot);
  if (result.error != NoError) {
    printf("Lot validation failed with error: %s\n",
           validation_error_message(result.error));
    lot_to_ppm_all_levels(lot, "invalid_lot", 30, NULL, 0);
    return 1;
  }

  // Create plateDB and read it from file
  char *PlateDBFileName = "test/test.txt";
  int lines = GetFileLines(PlateDBFileName);
  Car *CarArr = (Car *)malloc(sizeof(Car) * lines);

  ReadFile(CarArr, lines, PlateDBFileName);

  while (1) {

    // wait 3 seconds so any previous message is readable
    sleep_ms(3000);

    // then we clear the screen
    clear_screen();

    int box_width = 67;

    box_start(box_width);
    box_line("Welcome to the parking lot!", box_width);
    box_break(box_width);
    box_line_start();
    box_line_fill(
        printf("%d spaces available / %d total", lot.space_count - count_occupied_spaces(lot), lot.space_count),
        box_width);
    box_end(box_width);

    char TempPlate[8];
    if (scan_plate(TempPlate)) {
      printf("Plate is not valid. Please try again with a valid plate.\n");
      continue;
    }
    int CarIndex = GetCarIndexFromPlate(CarArr, lines, TempPlate);
    if (CarIndex == -1) {
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
    Space *assigned = NULL;
    CheckInResult checkin_result = handle_checkin(lot, CarArr[CarIndex], CarIndex, &assigned);
    if (checkin_result == EpicFail) {
      continue; // user couldn't get a space, error message already displayed
    }
    if (checkin_result == CheckOutSuccess) {
      continue; // user checked out successfully, no need to display navigation
    }
    if (assigned == NULL) {
      printf("No space was assigned, something went wrong.\n");
      continue;
    }

    // Print navigation instructions
    Space *foundSpace = assigned;
    int length = 0;
    Path *superpath = superpath_to_space(lot, *foundSpace, &length);
    if (superpath == NULL || length <= 0) {
      printf("No navigation path found to space %s.\n", foundSpace->name);
      continue;
    }
    lot_to_ppm(lot, "outImg.ppm", foundSpace->location.level, 30, superpath, length);
    printf(
        "Navigation path to space %s generated and saved as outImg.ppm.\n",
        foundSpace->name);
  }
  free(CarArr);
  return 0;
}
