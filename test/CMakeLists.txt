add_executable(test_1 test.c)
target_link_libraries(test_1 Unity)

add_executable(test_PlateDB PlateDB.c)
target_link_libraries(test_PlateDB data PlateDB Unity)

add_executable(test_lot lot.c)
target_link_libraries(test_lot lot Unity)

add_executable(test_data data.c)
target_link_libraries(test_data data calculations lot Unity)

add_executable(test_validate validate.c)
target_link_libraries(test_validate validate data lot calculations Unity)

add_executable(test_lotReader lotReader.c)
target_link_libraries(test_lotReader lotReader Unity)

add_executable(test_display display.c)
target_link_libraries(test_display display Unity)

add_executable(test_nav nav.c)
target_link_libraries(test_nav nav lotReader Unity)

add_test(NAME Test_1 COMMAND test_1)
add_test(NAME test_data COMMAND test_data)
add_test(NAME test_lot COMMAND test_lot)
add_test(NAME test_validate COMMAND test_validate)
add_test(NAME test_PlateDB COMMAND test_PlateDB)
add_test(NAME test_display COMMAND test_display)
add_test(NAME test_lotReader COMMAND test_lotReader)
