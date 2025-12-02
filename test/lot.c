#include "unity.h"
#include "lot.h"

void setUp() {}

void tearDown() {}

void test_create_lot(void) {
  int level_count = 2, path_count = 3, space_count = 30, up_count = 0, down_count = 0;
  Lot* lot = create_lot(level_count, path_count, space_count, up_count, down_count);
  TEST_ASSERT_EQUAL_INT_MESSAGE(level_count, lot->level_count, "create_lot should have used the correct level count"); 
  TEST_ASSERT_EQUAL_INT_MESSAGE(path_count, lot->path_count, "create_lot should have used the correct path count"); 
  TEST_ASSERT_EQUAL_INT_MESSAGE(space_count, lot->space_count, "create_lot should have used the correct space count"); 
  TEST_ASSERT_EQUAL_INT_MESSAGE(up_count, lot->up_count, "create_lot should have used the correct up count");
  TEST_ASSERT_EQUAL_INT_MESSAGE(down_count, lot->down_count, "create_lot should have used the correct down count");
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_create_lot);
	return UNITY_END();
}
