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
void test_superpath_to_space_paths_connected(void) {
  // Test that each path's endpoint connects to the next path's start point
  Space* space = space_by_name(lot, "C6");
  if (!space) { TEST_FAIL_MESSAGE("Space C6 not found in lot"); }
  
  int count = 0;
  Path* superpath = superpath_to_space(lot, *space, &count);
  
  TEST_ASSERT_TRUE_MESSAGE(count > 1, "Need at least 2 paths to test connectivity");
  TEST_ASSERT_NOT_NULL_MESSAGE(superpath, "Superpath should not be NULL");
  
  // Check that each path's endpoint matches the next path's start point
  for (int i = 0; i < count - 1; i++) {
  	printf("Current Path %d: start(%.2f, %.2f), vector(%.2f, %.2f)\n", 
			i, superpath[i].start_point.x, superpath[i].start_point.y,
			superpath[i].vector.x, superpath[i].vector.y);
    Location endpoint = get_endpoint(superpath[i]);
    Location next_start = superpath[i + 1].start_point;

    if (endpoint.level != next_start.level) {
      return; // Skip level mismatch checks
    }
    
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.001, endpoint.x, next_start.x, 
      "Path endpoint X should match next path's start X");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.001, endpoint.y, next_start.y, 
      "Path endpoint Y should match next path's start Y");
    TEST_ASSERT_EQUAL_INT_MESSAGE(endpoint.level, next_start.level, 
      "Path endpoint level should match next path's start level");
  }
  
  free(superpath);
}

int main(void) {
  UNITY_BEGIN();
 	RUN_TEST(test_superpath_to_space_paths_connected);
  return UNITY_END();
}
