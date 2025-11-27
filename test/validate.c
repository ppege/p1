#include "unity.h"
#include "validate.h"
#include "lot.h"
#include "data.h"

void setUp() {
}

void tearDown() {
}

void test_paths_connected() {
	Lot *lot = create_lot(2, 8, 0, 1, 1); // 2 levels, 8 paths, 1 up and 1 down. Ignoring spaces so far.
	print_lot(lot);

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
	TEST_ASSERT_EQUAL(1, paths_connected(lot));
	
	// Modify to create an unconnected path
	lot->paths[2].start_point = (Location){6, 3, 0};
	
	// Test unconnected paths
	TEST_ASSERT_EQUAL(0, paths_connected(lot));
	free_lot(lot);
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_paths_connected);
	return UNITY_END();
}
