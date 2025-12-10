add_library(data data.c)
target_include_directories(data PUBLIC .)

add_library(lot lot.c)
target_include_directories(lot PUBLIC .)
target_link_libraries(lot PUBLIC nav)

add_library(validate validate.c)
target_include_directories(validate PUBLIC .)
target_link_libraries(validate data)

add_library(calculations calculations.c)
target_include_directories(calculations PUBLIC .)
target_link_libraries(calculations PRIVATE m)

add_library(display display.c)
target_include_directories(display PUBLIC .)

add_library(PlateDB PlateDB.c)
target_include_directories(PlateDB PUBLIC .)

add_library(lotReader lotReader.c)
target_include_directories(lotReader PUBLIC .)
target_link_libraries(lotReader PUBLIC lot)

add_library(image image.c)
target_include_directories(image PUBLIC .)
target_link_libraries(image PUBLIC data lot calculations)

add_library(nav nav.c)
target_include_directories(nav PUBLIC .)
target_link_libraries(nav PUBLIC data calculations validate)

add_library(Unity STATIC external/Unity/src/unity.c)
target_include_directories(Unity PUBLIC external/Unity/src)
