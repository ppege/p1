add_library(data data.c)
target_include_directories(data PUBLIC .)

add_library(lot lot.c)
target_include_directories(lot PUBLIC .)

add_library(validate validate.c)
target_include_directories(validate PUBLIC .)

add_library(display display.c)
target_include_directories(display PUBLIC .)

add_library(PlateDB PlateDB.c)
target_include_directories(PlateDB PUBLIC .)

add_library(lotReader lotReader.c)
target_include_directories(lotReader PUBLIC .)

add_library(Unity STATIC external/Unity/src/unity.c)
target_include_directories(Unity PUBLIC external/Unity/src)
