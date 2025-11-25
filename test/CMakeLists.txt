add_executable(test_1 test.c)
target_link_libraries(test_1 data Unity)

add_executable(test_lot lot.c)
target_link_libraries(test_lot lot Unity)

add_test(NAME Test_1 COMMAND test_1)
add_test(NAME test_lot COMMAND test_lot)
