add_executable(main main.c)
target_link_libraries(main
                      image 
                      lotReader 
                      validate
                      PlateDB
                      data
                      display
                      lot)
