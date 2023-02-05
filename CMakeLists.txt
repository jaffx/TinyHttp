cmake_minimum_required(VERSION 3.23)
project(main)
set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
include_directories(${PROJECT_SOURCE_DIR}/xServer/include)
link_directories(${PROJECT_SOURCE_DIR}/xServer/lib)

add_executable(main main.cpp)
target_link_libraries(main xServer)