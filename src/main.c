#include "image.h"
#include "lot.h"
#include "lotReader.h"
#include "validate.h"
#include <stdio.h>

int main() {
  Lot *lot = create_lot(1, 1, 1, 1, 1);
  char *LotFileName = "example.lot";
  readLotFromFile(LotFileName, lot);
  ValidationResult result = validate_lot(lot);

  if (result.result == Err) {
    printf("Lot validation failed! %s\n",
           validation_error_message(result.error));
  }

  lot_to_ppm_all_levels(lot, "parking", 10);

  free_lot(lot);
  return 0;
}
