#include "unity.h"
#include "nav.h"
#include "lotReader.h"
#include "lot.h"
#include "data.h"
#include <stdlib.h>

static Lot lot;

void setUp() {
  lot = lot_from_file("../../test/test.lot");
}

void tearDown() {}

// === Test superpath_to_space ===

void test_superpath_to_space_nearby(void) {
  // Test finding a complete path from entrance to a nearby space
  Space* space = space_by_name(lot, "A3");
  if (!space) { TEST_FAIL_MESSAGE("Space A3 not found in lot"); }
  
  int count = 0;
  Path* superpath = superpath_to_space(lot, *space, &count);
  
  // Should find a valid path
  TEST_ASSERT_TRUE_MESSAGE(count > 0, "Should find a valid superpath to space A3");
  TEST_ASSERT_NOT_NULL_MESSAGE(superpath, "Superpath should not be NULL");
  
  // The first path should start at the entrance
  TEST_ASSERT_FLOAT_WITHIN(0.001, lot.entrance.x, superpath[0].start_point.x);
  TEST_ASSERT_FLOAT_WITHIN(0.001, lot.entrance.y, superpath[0].start_point.y);
  
  // The last path should end at the space location
  Location final_endpoint = get_endpoint(superpath[count - 1]);
  TEST_ASSERT_FLOAT_WITHIN(0.001, space->location.x, final_endpoint.x);
  TEST_ASSERT_FLOAT_WITHIN(0.001, space->location.y, final_endpoint.y);
  
  free(superpath);
}

void test_superpath_to_space_far(void) {
  // Test finding a path to a space further from entrance
  Space* space = space_by_name(lot, "B1");
  if (!space) { TEST_FAIL_MESSAGE("Space B1 not found in lot"); }
  
  int count = 0;
  Path* superpath = superpath_to_space(lot, *space, &count);
  
  // Should find a valid path
  TEST_ASSERT_TRUE_MESSAGE(count > 0, "Should find a valid superpath to space B1");
  TEST_ASSERT_NOT_NULL_MESSAGE(superpath, "Superpath should not be NULL");
  
  // Verify the path starts at entrance
  TEST_ASSERT_FLOAT_WITHIN(0.001, lot.entrance.x, superpath[0].start_point.x);
  TEST_ASSERT_FLOAT_WITHIN(0.001, lot.entrance.y, superpath[0].start_point.y);
  
  // Verify the path ends at the space
  Location final_endpoint = get_endpoint(superpath[count - 1]);
  TEST_ASSERT_FLOAT_WITHIN(0.001, space->location.x, final_endpoint.x);
  TEST_ASSERT_FLOAT_WITHIN(0.001, space->location.y, final_endpoint.y);
  
  free(superpath);
}

void test_superpath_to_space_right_side(void) {
  // Test a space on the right side of the lot
  Space* space = space_by_name(lot, "A6");
  if (!space) { TEST_FAIL_MESSAGE("Space A6 not found in lot"); }
  
  int count = 0;
  Path* superpath = superpath_to_space(lot, *space, &count);
  
  TEST_ASSERT_TRUE_MESSAGE(count > 0, "Should find a valid superpath to space A6");
  TEST_ASSERT_NOT_NULL_MESSAGE(superpath, "Superpath should not be NULL");
  
  free(superpath);
}

void test_superpath_to_space_shortest_path(void) {
  // Test that the function returns the shortest path
  // Space A4 at (6, 2) should be reached via the right branch, not the left
  Space* space = space_by_name(lot, "A4");
  if (!space) { TEST_FAIL_MESSAGE("Space A4 not found in lot"); }
  
  int count = 0;
  Path* superpath = superpath_to_space(lot, *space, &count);
  
  TEST_ASSERT_TRUE_MESSAGE(count > 0, "Should find a valid superpath");
  
  // Calculate total length - should be reasonably short
  double total_length = 0.0;
  for (int i = 0; i < count; i++) {
    double dx = superpath[i].vector.x;
    double dy = superpath[i].vector.y;
    total_length += sqrt(dx * dx + dy * dy);
  }
  
  // The shortest path to A4 at (6,2) from entrance (0,0) should be around 8-10 units
  // (up to y=2, then right to x=6) - definitely less than going all the way around
  TEST_ASSERT_TRUE_MESSAGE(total_length < 20.0, "Path should be reasonably short");
  
  free(superpath);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_superpath_to_space_nearby);
  RUN_TEST(test_superpath_to_space_far);
  RUN_TEST(test_superpath_to_space_right_side);
  RUN_TEST(test_superpath_to_space_shortest_path);
  return UNITY_END();
}
