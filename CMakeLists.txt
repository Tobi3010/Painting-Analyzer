cmake_minimum_required(VERSION 3.10)
project(MyOpenCVApp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)

find_package(OpenCV REQUIRED)

find_package(Qt5 REQUIRED COMPONENTS Widgets)


add_executable(main 
    main.cpp 
    grayscale.cpp 
    perspective.cpp 
    ClickLabel.cpp)

target_link_libraries(main PRIVATE ${OpenCV_LIBS} Qt5::Widgets)

# Separate test program
add_executable(test
    perspective.cpp 
    testPerspective.cpp
)
target_link_libraries(test PRIVATE ${OpenCV_LIBS} Qt5::Widgets)

