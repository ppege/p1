#include <stdio.h>
#include "lot.h"
#include "image.h"
#include "lotReader.h"
#include "validate.h"

int main() {
  char *filename = "diagonal.lot";
  Lot lot = lot_from_file(filename);
  ValidationResult result = validate_lot(lot);

  if (result.result == Err) {
    printf("Lot validation failed! %s\n", validation_error_message(result.error));
  }

  lot_to_ppm_all_levels(lot, "parking", 30);

  free_lot(lot);
  return 0;
}
