project(unit_tests)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/unit_tests")
set(CMAKE_AUTOMOC on)
set(CMAKE_INCLUDE_CURRENT_DIR on)
find_package(Qt5Test)

#--------------------------------------------------------------------------------
#                                               unit tests for the Rect template
#--------------------------------------------------------------------------------
set(test_name "rect-test")
add_executable(${test_name} rect_test.cpp ../src/Rect.h)
target_link_libraries(${test_name} Qt5::Test)
target_include_directories(${test_name} PRIVATE ../src ${CMAKE_CURRENT_BINARY_DIR})
add_test(NAME ${test_name} COMMAND ${test_name})

