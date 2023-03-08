cmake_minimum_required(VERSION 3.15)
project(Chess3D)

set(CMAKE_CXX_STANDARD 17)

find_package(threepp CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/models/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/data)


add_executable(MyTarget main.cpp)
target_link_libraries(MyTarget PRIVATE threepp::threepp imgui::imgui)
