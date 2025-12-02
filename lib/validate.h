#pragma once
#include <data.h>

typedef enum {
  Ok,
  Err
} Result;

typedef enum {
  NoError,
  ZeroLengthPath,
  PathNotConnected,
  SpacesOverlap,
  SpacesEncroachPath,
  SpacesInaccessible,
  InvalidEntranceOrPOI,
  DuplicateSpaceNames,
  IncorrectUpDownCount,
  LevelsMissingUpsOrDowns
} LotValidationError;

typedef struct {
  Result result;
  LotValidationError error;
} ValidationResult;

const char* validation_error_message(LotValidationError error);
ValidationResult validate_lot(const Lot *lot);
int paths_connected(const Lot *lot);
Location* get_all_endpoints(Path* paths, int path_count);
int compare_locations(Location loc1, Location loc2);
int spaces_overlap(const Lot *lot);
int spaces_encroach_path(const Lot *lot, double margin);
Rectangle get_path_corridor(const Path *path, double margin);
int spaces_accessible(const Lot *lot, double max_distance);
int has_valid_entrance_and_poi(const Lot *lot);
int spaces_have_unique_names(const Lot *lot);
int has_correct_up_down_count(const Lot *lot);
int levels_have_ups_and_downs(const Lot *lot);
