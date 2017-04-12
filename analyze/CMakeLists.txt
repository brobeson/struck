project("analyze")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

add_executable(${PROJECT_NAME}
    bounding_box.h
    ctype.h
    iou.h
    main.cpp)
