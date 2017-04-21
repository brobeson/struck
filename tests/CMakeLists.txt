project(unit_tests)

find_package(Boost)
if (NOT Boost_FOUND)
    message(WARNING "Boost wasn't found. Unit tests won't be available")
    return()
endif()

add_executable(rect_test rect_test.cpp)
target_link_libraries(rect_test ${Boost_LIBRARIES})
target_include_directories(rect_test PRIVATE ${struck_SOURCE_DIR})
add_test(NAME "Rectangle Test" COMMAND rect_test)
