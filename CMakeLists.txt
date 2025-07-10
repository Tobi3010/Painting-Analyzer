cmake_minimum_required(VERSION 3.10)
project(MyOpenCVApp)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(main main.cpp)

target_link_libraries(main PRIVATE ${OpenCV_LIBS} Qt5::Widgets)
