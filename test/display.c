#include "unity.h"
#include "display.h"

void setUp() {}
void tearDown() {}

// === Valid plates ===

void test_validate_plate_valid_uppercase() {
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, validate_plate("AB12345"), "AB12345 should be valid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, validate_plate("XY99999"), "XY99999 should be valid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, validate_plate("ZZ00000"), "ZZ00000 should be valid");
}

void test_validate_plate_valid_lowercase() {
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, validate_plate("ab12345"), "ab12345 should be valid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, validate_plate("xy99999"), "xy99999 should be valid");
}

void test_validate_plate_valid_mixed_case() {
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, validate_plate("Ab12345"), "Ab12345 should be valid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, validate_plate("aB12345"), "aB12345 should be valid");
}

// === Wrong length ===

void test_validate_plate_too_short() {
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("AB1234"), "6 chars should be invalid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("AB123"), "5 chars should be invalid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("AB"), "2 chars should be invalid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("A"), "1 char should be invalid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate(""), "empty should be invalid");
}

void test_validate_plate_too_long() {
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("AB123456"), "8 chars should be invalid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("AB1234567"), "9 chars should be invalid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("AB12345678901234"), "way too long should be invalid");
}

// === First two must be letters ===

void test_validate_plate_first_char_not_letter() {
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("1B12345"), "first char digit should be invalid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("9B12345"), "first char digit 9 should be invalid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("@B12345"), "first char @ should be invalid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate(" B12345"), "first char space should be invalid");
}

void test_validate_plate_second_char_not_letter() {
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("A112345"), "second char digit should be invalid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("A@12345"), "second char @ should be invalid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("A 12345"), "second char space should be invalid");
}

void test_validate_plate_both_first_chars_not_letters() {
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("1212345"), "both digits should be invalid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("@@12345"), "both special chars should be invalid");
}

// === Last five must be digits ===

void test_validate_plate_third_char_not_digit() {
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("ABA2345"), "letter at position 2 should be invalid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("AB 2345"), "space at position 2 should be invalid");
}

void test_validate_plate_middle_char_not_digit() {
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("AB12A45"), "letter in middle should be invalid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("AB12 45"), "space in middle should be invalid");
}

void test_validate_plate_last_char_not_digit() {
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("AB1234A"), "letter at end should be invalid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("AB1234! "), "!  at end should be invalid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("AB1234 "), "space at end should be invalid");
}

void test_validate_plate_all_letters() {
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("ABCDEFG"), "all letters should be invalid");
}

void test_validate_plate_all_digits() {
  TEST_ASSERT_EQUAL_INT_MESSAGE(1, validate_plate("1234567"), "all digits should be invalid");
}

// === Edge cases ===

void test_validate_plate_boundary_digits() {
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, validate_plate("AA00000"), "all zeros should be valid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, validate_plate("AA99999"), "all nines should be valid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, validate_plate("AA00001"), "leading zeros should be valid");
}

void test_validate_plate_boundary_letters() {
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, validate_plate("AA12345"), "AA should be valid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, validate_plate("ZZ12345"), "ZZ should be valid");
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, validate_plate("az12345"), "az lowercase should be valid");
}

int main(void) {
  UNITY_BEGIN();
  
  // Valid plates
  RUN_TEST(test_validate_plate_valid_uppercase);
  RUN_TEST(test_validate_plate_valid_lowercase);
  RUN_TEST(test_validate_plate_valid_mixed_case);
  
  // Wrong length
  RUN_TEST(test_validate_plate_too_short);
  RUN_TEST(test_validate_plate_too_long);
  
  // First two must be letters
  RUN_TEST(test_validate_plate_first_char_not_letter);
  RUN_TEST(test_validate_plate_second_char_not_letter);
  RUN_TEST(test_validate_plate_both_first_chars_not_letters);
  
  // Last five must be digits
  RUN_TEST(test_validate_plate_third_char_not_digit);
  RUN_TEST(test_validate_plate_middle_char_not_digit);
  RUN_TEST(test_validate_plate_last_char_not_digit);
  RUN_TEST(test_validate_plate_all_letters);
  RUN_TEST(test_validate_plate_all_digits);
  
  // Edge cases
  RUN_TEST(test_validate_plate_boundary_digits);
  RUN_TEST(test_validate_plate_boundary_letters);
  
  return UNITY_END();
}
