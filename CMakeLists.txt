# CMake version.
cmake_minimum_required(VERSION 2.8.11)

# The name of the project.
project(vision_feature_extraction)

# Used to resolve absolute path names
set(ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Use the c++11 standard
set (CMAKE_CXX_STANDARD 11)

# set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Use c++11 and check OpenCV 3.
SET(CMAKE_CXX_FLAGS "-g -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS_DEBUG} -g
							   -fno-omit-frame-pointer -fsanitize=address")
find_package(OpenCV 3 REQUIRED)

set(SOURCEFILES src/main.cpp src/binarization.hpp
	src/segmentation.hpp)

add_executable(vision_feature_extraction ${SOURCEFILES})

target_link_libraries(vision_feature_extraction ${OpenCV_LIBS})

message(${PROJECT_NAME} " done!")
