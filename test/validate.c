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

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_paths_connected);
	RUN_TEST(test_spaces_overlap);
	return UNITY_END();
}
