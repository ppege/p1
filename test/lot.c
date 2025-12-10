#include "unity.h"
#include "lot.h"
#include "lotReader.h"
#include "data.h"

void setUp() {}

void tearDown() {}

void test_create_lot(void) {
  int level_count = 2, path_count = 3, space_count = 30, up_count = 0, down_count = 0;
  Lot lot = create_lot(level_count, path_count, space_count, up_count, down_count);
  TEST_ASSERT_EQUAL_INT_MESSAGE(level_count, lot.level_count, "create_lot should have used the correct level count"); 
  TEST_ASSERT_EQUAL_INT_MESSAGE(path_count, lot.path_count, "create_lot should have used the correct path count"); 
  TEST_ASSERT_EQUAL_INT_MESSAGE(space_count, lot.space_count, "create_lot should have used the correct space count"); 
  TEST_ASSERT_EQUAL_INT_MESSAGE(up_count, lot.up_count, "create_lot should have used the correct up count");
  TEST_ASSERT_EQUAL_INT_MESSAGE(down_count, lot.down_count, "create_lot should have used the correct down count");
}

void test_best_space_no_occupancy(void) {
  Lot lot = lot_from_file("../../test/test.lot");
  Space* space = best_space(lot, Standard);
  TEST_ASSERT_NOT_NULL_MESSAGE(space, "best_space should return a valid space pointer");
  if (space != NULL) {
    TEST_ASSERT_EQUAL_STRING_MESSAGE("D3", space->name, "best_space should return the closest available Standard space named D3");
  } else {
    TEST_FAIL_MESSAGE("best_space returned NULL space pointer (somehow)");
  }
  free_lot(lot);
}

void test_best_space_partial_occupancy(void) {
  Lot lot = lot_from_file("../../test/test.lot");
  // Mark space C1 through D4 as occupied, leaving only D5 and D6 as available Standard spaces
  for (int i = 12; i < 22; i++) {
    lot.spaces[i].occupied = 1;
  }

  Space* space = best_space(lot, Standard);
  TEST_ASSERT_NOT_NULL_MESSAGE(space, "best_space should return a valid space pointer");
  if (space != NULL) {
    TEST_ASSERT_EQUAL_STRING_MESSAGE("D5", space->name, "best_space should return the closest available Standard space named D5");
  } else {
    TEST_FAIL_MESSAGE("best_space returned NULL space pointer (somehow)");
  }
  free_lot(lot);
}

void test_best_space_full_occupancy(void) {
  Lot lot = lot_from_file("../../test/test.lot");
  // occupy the whole lot
  for (int i = 0; i < lot.space_count; i++) {
    lot.spaces[i].occupied = 1;
  }

  Space* space = best_space(lot, Standard); // should return NULL since no spaces are available
  TEST_ASSERT_NULL_MESSAGE(space, "best_space should return NULL when no spaces are available");

  free_lot(lot);
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_create_lot);
	RUN_TEST(test_best_space_no_occupancy);
	RUN_TEST(test_best_space_partial_occupancy);
	RUN_TEST(test_best_space_full_occupancy);
	return UNITY_END();
}
