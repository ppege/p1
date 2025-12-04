#include "unity.h"
#include "nav.h"
#include "lotReader.h"
#include "lot.h"
#include <stdlib.h>

static Lot lot;

void setUp() {
  lot = lot_from_file("../../test/test.lot");
}

void tearDown() {}

void test_closest_point_on_path(void) {
	Space* space = space_by_name(lot, "A3");
	if (!space) { TEST_FAIL_MESSAGE("Space A3 not found in lot"); }
	Location closest_path0 = closest_point_on_path(lot.paths[0], *space);
	TEST_ASSERT_FLOAT_WITHIN(0.001, 0.0, closest_path0.x);
	TEST_ASSERT_FLOAT_WITHIN(0.001, 2.0, closest_path0.y);
	Location closest_path1 = closest_point_on_path(lot.paths[1], *space);
	TEST_ASSERT_FLOAT_WITHIN(0.001, -6.0, closest_path1.x);
	TEST_ASSERT_FLOAT_WITHIN(0.001, 10.0, closest_path1.y);
}

void test_available_paths(void) {
	double max_distance = 6.0;
	int count;

	Space* space = space_by_name(lot, "A3");
	if (!space) { TEST_FAIL_MESSAGE("Space A3 not found in lot"); }
	Path* paths = available_paths(lot, *space, max_distance, &count);

	TEST_ASSERT_EQUAL_INT(2, count);
	
	free(paths);
	free_lot(lot);
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_available_paths);
	RUN_TEST(test_closest_point_on_path);
	return UNITY_END();
}
