# Tests
In this project we do unit testing with the Unity test framework.
All of the tests is placed in the `./test` folder. Every file in the folder should test a library.
Don't put all the tests in just one file and test everything in there. Split it up like you would do with your code.
This way the test will be easier to manage.

# Structure
The structure is quite simple. The individual test files lays in `./test` folder. 
The file is then added to the `CMakeLists.txt` file as an executable and link the library you want to test, as well as the Unity test framework.
Also remember to add the test to CTest with the `add_test` function. The function takes a name, and a command (The name of the test executable).

# Running the tests
Make sure you first build the tests with the build command.
`````
```
cmake --build build 
```
After that you can run the tests, with CTest.
```
ctest --test-dir build
```
Then the tests should have been run succesfully.
