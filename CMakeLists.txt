cmake_minimum_required(VERSION 2.6)
project(struck)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, defaulting to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_HOME_DIRECTORY}/cmake ${CMAKE_MODULE_PATH})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Eigen REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED) # needed by VLFeat

include_directories(
    src
    ${EIGEN_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

# add all .cpp and .h files in src
file(GLOB_RECURSE SRC RELATIVE ${PROJECT_SOURCE_DIR} src/*.cpp)
file(GLOB_RECURSE HEADERS RELATIVE ${PROJECT_SOURCE_DIR} src/*.h)

add_executable(struck ${HEADERS} ${SRC})
target_include_directories(struck PRIVATE "/home/brendan/repositories/vlfeat/vlfeat/")

target_link_libraries(struck
    ${OpenCV_LIBS}
    Threads::Threads
    "/home/brendan/repositories/vlfeat/release/libvlfeat.a"
)

configure_file("config.txt" "bin/config.txt")
configure_file("run_experiments.sh" "bin/run_experiments.sh" @ONLY)
configure_file("demo.sh" "bin/demo.sh" @ONLY)

# build a tool to automate data analysis
add_subdirectory(analyze)

# build unit tests
enable_testing()
add_subdirectory(tests)
