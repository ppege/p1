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
  Lot* lot = create_lot(30, 3, 0, 0);
  TEST_ASSERT_EQUAL_INT(lot->space_count, 3); 
  TEST_ASSERT_EQUAL_INT(lot->path_count, 30); 
  TEST_ASSERT_EQUAL_INT(lot->up_count, 0); 
  TEST_ASSERT_EQUAL_INT(lot->down_count, 0); 
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_create_lot);
	return UNITY_END();
}
