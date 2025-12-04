#include "unity.h"
#include "validate.h"
#include "lot.h"
#include "data.h"
#include <string.h>

void setUp() {}

void tearDown() {}

// Helper macro for asserting ValidationResult
// This gives us nice error messages showing exactly which error occurred
#define TEST_ASSERT_VALIDATION_OK(lot) do { \
  ValidationResult _res = validate_lot(lot); \
  if (_res.result != Ok) { \
    char _msg[256]; \
    snprintf(_msg, sizeof(_msg), "Expected Ok but got error: %s", validation_error_message(_res.error)); \
    TEST_FAIL_MESSAGE(_msg); \
  } \
} while(0)

#define TEST_ASSERT_VALIDATION_ERR(expected_error, lot) do { \
  ValidationResult _res = validate_lot(lot); \
  if (_res.result != Err) { \
    TEST_FAIL_MESSAGE("Expected Err but got Ok"); \
  } else if (_res.error != expected_error) { \
    char _msg[256]; \
    snprintf(_msg, sizeof(_msg), "Expected error '%s' but got '%s'", \
      validation_error_message(expected_error), validation_error_message(_res.error)); \
    TEST_FAIL_MESSAGE(_msg); \
  } \
} while(0)

// === Rule 1: Each path must connect ===

void test_paths_connected() {
  Lot lot = create_lot(2, 8, 0, 1, 1);

  lot.paths[0] = (Path){ .start_point = (Location){5, 0, 0}, .vector = (Vector){0, 10} };
  lot.paths[1] = (Path){ .start_point = (Location){5, 0, 0}, .vector = (Vector){0, 3} };
  lot.paths[2] = (Path){ .start_point = (Location){5, 3, 0}, .vector = (Vector){-4, 0} };
  lot.paths[3] = (Path){ .start_point = (Location){5, 3, 0}, .vector = (Vector){4, 0} };
  lot.paths[4] = (Path){ .start_point = (Location){5, 10, 1}, .vector = (Vector){0, -10} };
  lot.paths[5] = (Path){ .start_point = (Location){5, 10, 1}, .vector = (Vector){0, -3} };
  lot.paths[6] = (Path){ .start_point = (Location){5, 7, 1}, .vector = (Vector){-4, 0} };
  lot.paths[7] = (Path){ .start_point = (Location){5, 7, 1}, .vector = (Vector){4, 0} };
  lot.entrance = (Location){5, 0, 0};
  lot.POI = (Location){-100, 0, 0};
  lot.ups[0] = (Location){5, 10, 0};
  lot.downs[0] = (Location){5, 10, 1};

  TEST_ASSERT_EQUAL_INT_MESSAGE(1, paths_connected(lot), "paths should be connected here");
  
  lot.paths[2].start_point = (Location){6, 3, 0};
  
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, paths_connected(lot), "paths should have an orphan path here");
  free_lot(lot);
}

// === Rule 2: No spaces may overlap ===

void test_spaces_overlap() {
  Lot lot = create_lot(1, 0, 2, 0, 0);
  
  lot.spaces[0] = (Space){ .type = Standard, .location = (Location){5, 5, 0}, .rotation = 315.0, .name = "Space1" };
  lot.spaces[1] = (Space){ .type = Standard, .location = (Location){10, 5, 0}, .rotation = 0.0, .name = "Space2" };
  
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, spaces_overlap(lot), "spaces should overlap here");

  lot.spaces[1].rotation = 315.0;
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, spaces_overlap(lot), "spaces should not overlap here");
  free_lot(lot);
}

// === Rule 3: No obstacles (spaces) must encroach within PATH_CLEARANCE meters of either side of the path centerline ===

void test_spaces_encroach_path() {
  Lot lot = create_lot(1, 2, 3, 0, 0);
  double margin = 1.5;
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){10, 10} };
  lot.paths[1] = (Path){ .start_point = (Location){10, 10, 0}, .vector = (Vector){5, -5} };
  lot.spaces[0] = (Space){ .type = Standard, .location = (Location){12, 12, 0}, .rotation = 315.0, .name = "space0" };
  lot.spaces[1] = (Space){ .type = Standard, .location = (Location){12, 6, 0}, .rotation = 225.0, .name = "space1" };
  lot.spaces[2] = (Space){ .type = Standard, .location = (Location){8, 2, 0}, .rotation = 225.0, .name = "space2" };
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, spaces_encroach_path(lot, margin), "space should encroach the path here");

  lot.spaces[1].location = (Location){12, 5.5, 0};
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, spaces_encroach_path(lot, margin), "space should not encroach the path here");

  lot.spaces[1].location = (Location){12, 7, 0};
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, spaces_encroach_path(lot, margin), "space on path centerline should definitely encroach");

  free_lot(lot);
}

// === Rule 4: Spaces must be within PATH_ACCESSIBILITY of a path ===

void test_spaces_accessible() {
  Lot lot = create_lot(1, 2, 4, 0, 0);
  double max_distance = 6.0;
  
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){20, 0} };
  lot.paths[1] = (Path){ .start_point = (Location){20, 0, 0}, .vector = (Vector){0, 20} };
  
  lot.spaces[0] = (Space){ .type = Standard, .location = (Location){5, 3, 0}, .rotation = 0.0, .name = "close1" };
  lot.spaces[1] = (Space){ .type = Standard, .location = (Location){10, 5, 0}, .rotation = 0.0, .name = "close2" };
  lot.spaces[2] = (Space){ .type = Standard, .location = (Location){22, 10, 0}, .rotation = 0.0, .name = "close3" };
  lot.spaces[3] = (Space){ .type = Standard, .location = (Location){15, 4, 0}, .rotation = 0.0, .name = "close4" };
  
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, spaces_accessible(lot, max_distance), "all spaces should be within 6m of a path");
  
  lot.spaces[3] = (Space){ .type = Standard, .location = (Location){10, 15, 0}, .rotation = 0.0, .name = "far" };
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, spaces_accessible(lot, max_distance), "space 'far' should be inaccessible");
  
  lot.spaces[3] = (Space){ .type = Standard, .location = (Location){15, 4, 1}, .rotation = 0.0, .name = "wrong_level" };
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, spaces_accessible(lot, max_distance), "space on different level is not accessible");
  
  free_lot(lot);
}

// === Rule 5: Valid entrance and POI ===

void test_has_valid_entrance_and_poi() {
  Lot lot = create_lot(2, 1, 0, 0, 0);
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){10, 0} };
  
  lot.entrance = (Location){0, 0, 0};
  lot.POI = (Location){10, 0, 0};
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, has_valid_entrance_and_poi(lot), "entrance and POI on valid level 0");
  
  lot.entrance = (Location){0, 0, 0};
  lot.POI = (Location){10, 0, 1};
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, has_valid_entrance_and_poi(lot), "entrance level 0, POI level 1");
  
  lot.entrance = (Location){0, 0, 5};
  lot.POI = (Location){10, 0, 0};
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, has_valid_entrance_and_poi(lot), "entrance on invalid level 5");
  
  lot.entrance = (Location){0, 0, 0};
  lot.POI = (Location){10, 0, 3};
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, has_valid_entrance_and_poi(lot), "POI on invalid level 3");
  
  lot.entrance = (Location){0, 0, -1};
  lot.POI = (Location){10, 0, 0};
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, has_valid_entrance_and_poi(lot), "entrance on negative level");
  
  free_lot(lot);
}

// === Rule 6: Unique space names ===

void test_spaces_have_unique_names() {
  Lot lot = create_lot(1, 1, 4, 0, 0);
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){20, 0} };
  
  lot.spaces[0] = (Space){ .type = Standard, .location = (Location){1, 3, 0}, .rotation = 0, .name = "A1" };
  lot.spaces[1] = (Space){ .type = Standard, .location = (Location){4, 3, 0}, .rotation = 0, .name = "A2" };
  lot.spaces[2] = (Space){ .type = Standard, .location = (Location){7, 3, 0}, .rotation = 0, .name = "B1" };
  lot.spaces[3] = (Space){ .type = Standard, .location = (Location){10, 3, 0}, .rotation = 0, .name = "B2" };
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, spaces_have_unique_names(lot), "all names should be unique");
  
  lot.spaces[3].name = "A1";
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, spaces_have_unique_names(lot), "first and last should have same name");
  
  lot.spaces[3].name = "B2";
  lot.spaces[1].name = "A1";
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, spaces_have_unique_names(lot), "adjacent spaces should have same name");
  
  free_lot(lot);
}

void test_spaces_have_unique_names_empty() {
  Lot lot = create_lot(1, 1, 0, 0, 0);
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){10, 0} };
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, spaces_have_unique_names(lot), "no spaces means no duplicates");
  free_lot(lot);
}

void test_spaces_have_unique_names_single() {
  Lot lot = create_lot(1, 1, 1, 0, 0);
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){10, 0} };
  lot.spaces[0] = (Space){ .type = Standard, .location = (Location){1, 3, 0}, .rotation = 0, .name = "A1" };
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, spaces_have_unique_names(lot), "single space is always unique");
  free_lot(lot);
}

// === Rule 7: Correct up/down count ===

void test_has_correct_up_down_count_single_level() {
  Lot lot = create_lot(1, 1, 0, 0, 0);
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){10, 0} };
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, has_correct_up_down_count(lot), "single level needs no ups/downs");
  free_lot(lot);
}

void test_has_correct_up_down_count_two_levels() {
  Lot lot = create_lot(2, 1, 0, 1, 1);
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){10, 0} };
  lot.ups[0] = (Location){5, 0, 0};
  lot.downs[0] = (Location){5, 0, 1};
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, has_correct_up_down_count(lot), "2 levels needs 1 up and 1 down");
  free_lot(lot);
}

void test_has_correct_up_down_count_three_levels() {
  Lot lot = create_lot(3, 1, 0, 2, 2);
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){10, 0} };
  lot.ups[0] = (Location){5, 0, 0};
  lot.ups[1] = (Location){5, 0, 1};
  lot.downs[0] = (Location){5, 0, 1};
  lot.downs[1] = (Location){5, 0, 2};
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, has_correct_up_down_count(lot), "3 levels needs 2 ups and 2 downs");
  free_lot(lot);
}

void test_has_correct_up_down_count_missing_up() {
  Lot lot = create_lot(2, 1, 0, 0, 1);
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){10, 0} };
  lot.downs[0] = (Location){5, 0, 1};
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, has_correct_up_down_count(lot), "missing up");
  free_lot(lot);
}

void test_has_correct_up_down_count_missing_down() {
  Lot lot = create_lot(2, 1, 0, 1, 0);
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){10, 0} };
  lot.ups[0] = (Location){5, 0, 0};
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, has_correct_up_down_count(lot), "missing down");
  free_lot(lot);
}

void test_has_correct_up_down_count_too_many() {
  Lot lot = create_lot(2, 1, 0, 3, 1);
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){10, 0} };
  lot.ups[0] = (Location){5, 0, 0};
  lot.ups[1] = (Location){6, 0, 0};
  lot.ups[2] = (Location){7, 0, 0};
  lot.downs[0] = (Location){5, 0, 1};
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, has_correct_up_down_count(lot), "too many ups");
  free_lot(lot);
}

// === Rule 8: Each level has appropriate ups/downs ===

void test_levels_have_ups_and_downs_single_level() {
  Lot lot = create_lot(1, 1, 0, 0, 0);
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){10, 0} };
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, levels_have_ups_and_downs(lot), "single level needs no ups/downs");
  free_lot(lot);
}

void test_levels_have_ups_and_downs_two_levels_valid() {
  Lot lot = create_lot(2, 1, 0, 1, 1);
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){10, 0} };
  lot.ups[0] = (Location){5, 0, 0};
  lot.downs[0] = (Location){5, 0, 1};
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, levels_have_ups_and_downs(lot), "level 0 has up, level 1 has down");
  free_lot(lot);
}

void test_levels_have_ups_and_downs_three_levels_valid() {
  Lot lot = create_lot(3, 1, 0, 2, 2);
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){10, 0} };
  lot.ups[0] = (Location){5, 0, 0};
  lot.ups[1] = (Location){5, 0, 1};
  lot.downs[0] = (Location){5, 0, 1};
  lot.downs[1] = (Location){5, 0, 2};
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, levels_have_ups_and_downs(lot), "all levels correctly connected");
  free_lot(lot);
}

void test_levels_have_ups_and_downs_missing_up_on_level_0() {
  Lot lot = create_lot(2, 1, 0, 1, 1);
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){10, 0} };
  lot.ups[0] = (Location){5, 0, 1};
  lot.downs[0] = (Location){5, 0, 1};
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, levels_have_ups_and_downs(lot), "level 0 has no up");
  free_lot(lot);
}

void test_levels_have_ups_and_downs_missing_down_on_top() {
  Lot lot = create_lot(2, 1, 0, 1, 1);
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){10, 0} };
  lot.ups[0] = (Location){5, 0, 0};
  lot.downs[0] = (Location){5, 0, 0};
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, levels_have_ups_and_downs(lot), "level 1 has no down");
  free_lot(lot);
}

void test_levels_have_ups_and_downs_middle_level_missing_up() {
  Lot lot = create_lot(3, 1, 0, 2, 2);
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){10, 0} };
  lot.ups[0] = (Location){5, 0, 0};
  lot.ups[1] = (Location){6, 0, 0};
  lot.downs[0] = (Location){5, 0, 1};
  lot.downs[1] = (Location){5, 0, 2};
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, levels_have_ups_and_downs(lot), "level 1 has no up");
  free_lot(lot);
}

// === Full validate_lot() integration tests using ValidationResult ===

void test_validate_lot_valid() {
  Lot lot = create_lot(2, 4, 4, 1, 1);
  
  lot.entrance = (Location){0, 0, 0};
  lot.POI = (Location){20, 0, 0};
  
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){20, 0} };
  lot.paths[1] = (Path){ .start_point = (Location){20, 0, 0}, .vector = (Vector){0, 20} };
  lot.paths[2] = (Path){ .start_point = (Location){0, 0, 1}, .vector = (Vector){20, 0} };
  lot.paths[3] = (Path){ .start_point = (Location){20, 0, 1}, .vector = (Vector){0, 20} };
  
  lot.ups[0] = (Location){0, 0, 0};
  lot.downs[0] = (Location){0, 0, 1};
  
  lot.spaces[0] = (Space){ .type = Standard, .location = (Location){5, 4, 0}, .rotation = 0, .name = "A1" };
  lot.spaces[1] = (Space){ .type = Standard, .location = (Location){10, 4, 0}, .rotation = 0, .name = "A2" };
  lot.spaces[2] = (Space){ .type = Standard, .location = (Location){5, 4, 1}, .rotation = 0, .name = "B1" };
  lot.spaces[3] = (Space){ .type = Standard, .location = (Location){10, 4, 1}, .rotation = 0, .name = "B2" };
  
  TEST_ASSERT_VALIDATION_OK(lot);
  
  free_lot(lot);
}

void test_validate_lot_zero_length_path() {
  Lot lot = create_lot(1, 1, 0, 0, 0);
  lot.entrance = (Location){0, 0, 0};
  lot.POI = (Location){0, 0, 0};
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){0, 0} };
  
  TEST_ASSERT_VALIDATION_ERR(ZeroLengthPath, lot);
  
  free_lot(lot);
}

void test_validate_lot_disconnected_path() {
  Lot lot = create_lot(1, 2, 0, 0, 0);
  lot.entrance = (Location){0, 0, 0};
  lot.POI = (Location){10, 0, 0};
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){10, 0} };
  lot.paths[1] = (Path){ .start_point = (Location){50, 50, 0}, .vector = (Vector){10, 0} };
  
  TEST_ASSERT_VALIDATION_ERR(PathNotConnected, lot);
  
  free_lot(lot);
}

void test_validate_lot_overlapping_spaces() {
  Lot lot = create_lot(1, 1, 2, 0, 0);
  lot.entrance = (Location){0, 0, 0};
  lot.POI = (Location){10, 0, 0};
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){20, 0} };
  
  lot.spaces[0] = (Space){ .type = Standard, .location = (Location){5, 4, 0}, .rotation = 0, .name = "A1" };
  lot.spaces[1] = (Space){ .type = Standard, .location = (Location){5, 4, 0}, .rotation = 0, .name = "A2" };
  
  TEST_ASSERT_VALIDATION_ERR(SpacesOverlap, lot);
  
  free_lot(lot);
}

void test_validate_lot_space_encroaches_path() {
  Lot lot = create_lot(1, 1, 1, 0, 0);
  lot.entrance = (Location){0, 0, 0};
  lot.POI = (Location){10, 0, 0};
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){20, 0} };
  
  lot.spaces[0] = (Space){ .type = Standard, .location = (Location){5, 0, 0}, .rotation = 0, .name = "A1" };
  
  TEST_ASSERT_VALIDATION_ERR(SpacesEncroachPath, lot);
  
  free_lot(lot);
}

void test_validate_lot_space_not_accessible() {
  Lot lot = create_lot(1, 1, 1, 0, 0);
  lot.entrance = (Location){0, 0, 0};
  lot.POI = (Location){10, 0, 0};
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){20, 0} };
  
  lot.spaces[0] = (Space){ .type = Standard, .location = (Location){5, 50, 0}, .rotation = 0, .name = "A1" };
  
  TEST_ASSERT_VALIDATION_ERR(SpacesInaccessible, lot);
  
  free_lot(lot);
}

void test_validate_lot_invalid_entrance() {
  Lot lot = create_lot(1, 1, 1, 0, 0);
  lot.entrance = (Location){0, 0, 5};
  lot.POI = (Location){10, 0, 0};

  // the path needs to start at level 5 otherwise we get a different error message lol
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 5}, .vector = (Vector){20, 0} }; 

	// same goes for the space lmaoooo
  lot.spaces[0] = (Space){ .type = Standard, .location = (Location){5, 4, 5}, .rotation = 0, .name = "A1" };
  
  TEST_ASSERT_VALIDATION_ERR(InvalidEntranceOrPOI, lot);
  
  free_lot(lot);
}

void test_validate_lot_invalid_poi() {
  Lot lot = create_lot(1, 1, 1, 0, 0);
  lot.entrance = (Location){0, 0, 0};
  lot.POI = (Location){10, 0, 99};
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){20, 0} };
  lot.spaces[0] = (Space){ .type = Standard, .location = (Location){5, 4, 0}, .rotation = 0, .name = "A1" };
  
  TEST_ASSERT_VALIDATION_ERR(InvalidEntranceOrPOI, lot);
  
  free_lot(lot);
}

void test_validate_lot_duplicate_space_names() {
  Lot lot = create_lot(1, 1, 2, 0, 0);
  lot.entrance = (Location){0, 0, 0};
  lot.POI = (Location){10, 0, 0};
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){20, 0} };
  
  lot.spaces[0] = (Space){ .type = Standard, .location = (Location){5, 4, 0}, .rotation = 0, .name = "A1" };
  lot.spaces[1] = (Space){ .type = Standard, .location = (Location){10, 4, 0}, .rotation = 0, .name = "A1" };
  
  TEST_ASSERT_VALIDATION_ERR(DuplicateSpaceNames, lot);
  
  free_lot(lot);
}

void test_validate_lot_wrong_up_down_count() {
  Lot lot = create_lot(2, 2, 2, 1, 2);
  lot.entrance = (Location){0, 0, 0};
  lot.POI = (Location){10, 0, 0};
  lot.ups[0] = (Location){0, 0, 0};
	lot.downs[0] = (Location){0, 0, 1};
	lot.downs[1] = (Location){50, 50, 1}; // to hell with you, invalid down!
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){20, 0} };
  lot.paths[1] = (Path){ .start_point = (Location){0, 0, 1}, .vector = (Vector){20, 0} };
  lot.spaces[0] = (Space){ .type = Standard, .location = (Location){5, 4, 0}, .rotation = 0, .name = "A1" };
  lot.spaces[1] = (Space){ .type = Standard, .location = (Location){5, 4, 1}, .rotation = 0, .name = "B1" };
  
  TEST_ASSERT_VALIDATION_ERR(IncorrectUpDownCount, lot);
  
  free_lot(lot);
}

void test_validate_lot_ups_downs_on_wrong_levels() {
  // 3 levels to test middle level missing something
  Lot lot = create_lot(3, 3, 3, 2, 2);
  lot.entrance = (Location){0, 0, 0};
  lot.POI = (Location){10, 0, 0};
  
  // Paths on each level
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){20, 0} };   // level 0, from entrance
  lot.paths[1] = (Path){ .start_point = (Location){20, 0, 1}, .vector = (Vector){-20, 0} }; // level 1, from down
  lot.paths[2] = (Path){ .start_point = (Location){20, 0, 2}, .vector = (Vector){-20, 0} }; // level 2, from down
  
  lot.spaces[0] = (Space){ .type = Standard, .location = (Location){5, 4, 0}, .rotation = 0, .name = "A1" };
  lot.spaces[1] = (Space){ .type = Standard, .location = (Location){5, 4, 1}, .rotation = 0, .name = "B1" };
  lot.spaces[2] = (Space){ .type = Standard, .location = (Location){5, 4, 2}, .rotation = 0, .name = "C1" };
  
  // Ups: level 0 and level 1 need ups (to go to level 1 and 2)
  lot.ups[0] = (Location){20, 0, 0};  // up on level 0 ✓
  lot.ups[1] = (Location){20, 0, 0};  // WRONG: another up on level 0, level 1 has no up!
  
  // Downs: level 1 and level 2 need downs (to go back to level 0 and 1)
  lot.downs[0] = (Location){20, 0, 1}; // down on level 1 ✓
  lot.downs[1] = (Location){20, 0, 2}; // down on level 2 ✓
  
  TEST_ASSERT_VALIDATION_ERR(LevelsMissingUpsOrDowns, lot);
  
  free_lot(lot);
}

void test_validate_lot_no_spaces() {
  Lot lot = create_lot(1, 1, 0, 0, 0);
  lot.entrance = (Location){0, 0, 0};
  lot.POI = (Location){10, 0, 0};
  lot.paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){20, 0} };
  
  TEST_ASSERT_VALIDATION_OK(lot);
  
  free_lot(lot);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_paths_connected);
  RUN_TEST(test_spaces_overlap);
  RUN_TEST(test_spaces_encroach_path);
  RUN_TEST(test_spaces_accessible);
  RUN_TEST(test_has_valid_entrance_and_poi);
  RUN_TEST(test_spaces_have_unique_names);
  RUN_TEST(test_spaces_have_unique_names_empty);
  RUN_TEST(test_spaces_have_unique_names_single);
  RUN_TEST(test_has_correct_up_down_count_single_level);
  RUN_TEST(test_has_correct_up_down_count_two_levels);
  RUN_TEST(test_has_correct_up_down_count_three_levels);
  RUN_TEST(test_has_correct_up_down_count_missing_up);
  RUN_TEST(test_has_correct_up_down_count_missing_down);
  RUN_TEST(test_has_correct_up_down_count_too_many);
  RUN_TEST(test_levels_have_ups_and_downs_single_level);
  RUN_TEST(test_levels_have_ups_and_downs_two_levels_valid);
  RUN_TEST(test_levels_have_ups_and_downs_three_levels_valid);
  RUN_TEST(test_levels_have_ups_and_downs_missing_up_on_level_0);
  RUN_TEST(test_levels_have_ups_and_downs_missing_down_on_top);
  RUN_TEST(test_levels_have_ups_and_downs_middle_level_missing_up);
  RUN_TEST(test_validate_lot_valid);
  RUN_TEST(test_validate_lot_zero_length_path);
  RUN_TEST(test_validate_lot_disconnected_path);
  RUN_TEST(test_validate_lot_overlapping_spaces);
  RUN_TEST(test_validate_lot_space_encroaches_path);
  RUN_TEST(test_validate_lot_space_not_accessible);
  RUN_TEST(test_validate_lot_invalid_entrance);
  RUN_TEST(test_validate_lot_invalid_poi);
  RUN_TEST(test_validate_lot_duplicate_space_names);
  RUN_TEST(test_validate_lot_wrong_up_down_count);
  RUN_TEST(test_validate_lot_ups_downs_on_wrong_levels);
  RUN_TEST(test_validate_lot_no_spaces);
  return UNITY_END();
}
