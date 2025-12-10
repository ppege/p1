#include "PlateDB.h"
#include "data.h"
#include "display.h"
#include "image.h"
#include "lot.h"
#include "lotReader.h"
#include "validate.h"
#include "nav.h"
#include "data.h"

int main() {
  Lot lot = lot_from_file("diagonal.lot");
  ValidationResult result = validate_lot(lot);

  if (result.error != NoError) {
    printf("Lot validation failed with error: %s\n",
           validation_error_message(result.error));
    return 1;
  }

  Space* space = space_by_name(lot, "B5");
  if (space == NULL) {
    free_lot(lot);
    return 1;
  }

  int count = 0;
  Path* nav = superpath_to_space(lot, *space, &count);

  int standard_size = 30;
  lot_to_ppm_all_levels(lot, "parking", standard_size, nav, count);

  free(CarArr);
  return 0;
}
