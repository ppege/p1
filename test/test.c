#include "unity.h"
#include "data.h"

void setUp()
{
}

void tearDown()
{
}

void test_add_2_and_2(void)
{
	TEST_ASSERT_EQUAL_INT(4, addfn(2, 2));
}
int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_add_2_and_2);
	return UNITY_END();
}
