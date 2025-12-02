#include "unity.h"
#include "validate.h"
#include "lot.h"
#include "data.h"

void setUp() {}

void tearDown() {}

void test_paths_connected() {
	Lot *lot = create_lot(2, 8, 0, 1, 1); // 2 levels, 8 paths, 1 up and 1 down. Ignoring spaces so far.

	// magic number heaven
	lot->paths[0] = (Path){ .start_point = (Location){5, 0, 0}, .vector = (Vector){0, 10} }; // 0th floor vector from the entrance to an up
	lot->paths[1] = (Path){ .start_point = (Location){5, 0, 0}, .vector = (Vector){0, 3} }; // 0th floor vector on top of the first path, with a lower endpoint
	lot->paths[2] = (Path){ .start_point = (Location){5, 3, 0}, .vector = (Vector){-4, 0} }; // 0th floor vector from the second paths endpoint to the left
	lot->paths[3] = (Path){ .start_point = (Location){5, 3, 0}, .vector = (Vector){4, 0} }; // 0th floor vector from the second paths endpoint to the right
	lot->paths[4] = (Path){ .start_point = (Location){5, 10, 1}, .vector = (Vector){0, -10} }; // 1st floor vector downwards from a down
	lot->paths[5] = (Path){ .start_point = (Location){5, 10, 1}, .vector = (Vector){0, -3} }; // 1st floor vector on top of the previous path, with a higher endpoint
	lot->paths[6] = (Path){ .start_point = (Location){5, 7, 1}, .vector = (Vector){-4, 0} }; // 1st floor vector from the previous paths endpoint to the left
	lot->paths[7] = (Path){ .start_point = (Location){5, 7, 1}, .vector = (Vector){4, 0} }; // 1st floor vector from the previous previous paths endpoint to the right
	lot->entrance = (Location){5, 0, 0};
	lot->POI = (Location){-100, 0, 0};
	lot->ups[0] = (Location){5, 10, 0};
	lot->downs[0] = (Location){5, 10, 1};

	// Test connected paths
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, paths_connected(lot), "paths should be connected here");
	
	// Modify to create an disconnect
	lot->paths[2].start_point = (Location){6, 3, 0};
	
	// Test unconnected paths
	TEST_ASSERT_EQUAL_INT_MESSAGE(0, paths_connected(lot), "paths should have an orphan path here");
	free_lot(lot);
}

void test_spaces_overlap() {
	Lot *lot = create_lot(1, 0, 2, 0, 0); // 1 level, 0 paths, 2 spaces
	
	// we assign the two spaces in a way where they overlap
	lot->spaces[0] = (Space){ .type = Standard, .location = (Location){5, 5, 0}, .rotation = 315.0, .name = "Space1" };
	lot->spaces[1] = (Space){ .type = Standard, .location = (Location){10, 5, 0}, .rotation = 0.0, .name = "Space2" }; // Overlaps with Space1
	
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, spaces_overlap(lot), "spaces should overlap here");

	// matching Space2's rotation to Space1's rotation makes them be right next to each other
	lot->spaces[1].rotation = 315.0;
	TEST_ASSERT_EQUAL_INT_MESSAGE(0, spaces_overlap(lot), "spaces should not overlap here");
	free_lot(lot);
}

void test_spaces_encroach_path() {
	Lot *lot = create_lot(1, 2, 3, 0, 0); // 1 level, 2 paths, 3 spaces
	double margin = 1.5;
	lot->paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){10, 10} }; // 45° up right from origo
	lot->paths[1] = (Path){ .start_point = (Location){10, 10, 0}, .vector = (Vector){5, -5} }; // 45° down right from (10,10)
	lot->spaces[0] = (Space){ .type = Standard, .location = (Location){12, 12, 0}, .rotation = 315.0, .name = "space0" };
	lot->spaces[1] = (Space){ .type = Standard, .location = (Location){12, 6, 0}, .rotation = 225.0, .name = "space1" };
	lot->spaces[2] = (Space){ .type = Standard, .location = (Location){8, 2, 0}, .rotation = 225.0, .name = "space2" };
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, spaces_encroach_path(lot, margin), "space should encroach the path here");

	// we must move space1 just a smidge down, because the offset distance on the x and y axes
	// depends on the path's angle. The actual perpendicular distance is still 1.5m,
	// but for a 45° path, the offset is split: (1.06, 1.06) where sqrt(1.06² + 1.06²) is about 1.5. (diabolical)
	lot->spaces[1].location = (Location){12, 5.5, 0};
	TEST_ASSERT_EQUAL_INT_MESSAGE(0, spaces_encroach_path(lot, margin), "space should not encroach the path here");

	// Test obvious encroachment: place a space directly on the path
	lot->spaces[1].location = (Location){12, 7, 0}; // directly on path 1 centerline (y = -x + 20, at x=12 -> y=8)
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, spaces_encroach_path(lot, margin), "space on path centerline should definitely encroach");

	free_lot(lot);
}

void test_spaces_accessible() {
    Lot *lot = create_lot(1, 2, 4, 0, 0); // 2 levels, 2 paths, 4 spaces
    double max_distance = 6.0;
    
    // two paths forming an L shape
    lot->paths[0] = (Path){ .start_point = (Location){0, 0, 0}, .vector = (Vector){20, 0} };   // horizontal path along x-axis
    lot->paths[1] = (Path){ .start_point = (Location){20, 0, 0}, .vector = (Vector){0, 20} }; // vertical path going up
    
    // spaces within 6m of a path - should be accessible
    lot->spaces[0] = (Space){ .type = Standard, .location = (Location){5, 3, 0}, .rotation = 0.0, .name = "close1" };   // 3m from path 0
    lot->spaces[1] = (Space){ .type = Standard, .location = (Location){10, 5, 0}, .rotation = 0.0, .name = "close2" };  // 5m from path 0
    lot->spaces[2] = (Space){ .type = Standard, .location = (Location){22, 10, 0}, .rotation = 0.0, .name = "close3" }; // 2m from path 1
    lot->spaces[3] = (Space){ .type = Standard, .location = (Location){15, 4, 0}, .rotation = 0.0, .name = "close4" };  // 4m from path 0
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, spaces_accessible(lot, max_distance), "all spaces should be within 6m of a path");
    
    // move one space too far away - should fail
    lot->spaces[3] = (Space){ .type = Standard, .location = (Location){10, 15, 0}, .rotation = 0.0, .name = "far" }; // ~10m from nearest path
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, spaces_accessible(lot, max_distance), "space 'far' should be inaccessible");
    
    // move it back, but to a different level. should fail (no paths on level 1)
    lot->spaces[3] = (Space){ .type = Standard, .location = (Location){15, 4, 1}, .rotation = 0.0, .name = "wrong_level" };
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, spaces_accessible(lot, max_distance), "space on different level is not accessible");
    
    free_lot(lot);
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_paths_connected);
	RUN_TEST(test_spaces_overlap);
	RUN_TEST(test_spaces_encroach_path);
	RUN_TEST(test_spaces_accessible);
	return UNITY_END();
}
