#include "unity.h"
#include "lot.h"

void setUp()
{
}

void tearDown()
{
}

void test_create_lot(void)
{
  int path_count = 3;
  int space_count = 30;
  int up_count = 0;
  int down_count = 0;
  Lot* lot = create_lot(path_count, space_count, up_count, down_count);
  TEST_ASSERT_EQUAL_INT(path_count, lot->path_count); 
  TEST_ASSERT_EQUAL_INT(space_count, lot->space_count); 
  TEST_ASSERT_EQUAL_INT(up_count, lot->up_count); 
  TEST_ASSERT_EQUAL_INT(down_count, lot->down_count);
  // TEST_ASSERT_EQUAL_INT(path_count * sizeof(Path), sizeof(lot->paths));
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_create_lot);
	return UNITY_END();
}
