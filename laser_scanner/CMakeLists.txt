cmake_minimum_required(VERSION 3.0.2)
project(laser_scanner)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-lusb-1.0)

## Find catkin and any catkin packages
find_package(catkin REQUIRED COMPONENTS roscpp rospy std_msgs genmsg)

## Declare ROS messages and services

## Generate added messages and services
generate_messages(DEPENDENCIES std_msgs)

## Declare a catkin package
catkin_package()

## Build talker and listener
include_directories(include ${catkin_INCLUDE_DIRS})

add_executable(laser_publisher src/laser_publisher.cpp)
target_link_libraries(laser_publisher ${catkin_LIBRARIES} /usr/lib/x86_64-linux-gnu/libusb-1.0.so)
add_dependencies(laser_publisher laser_scanner_generate_messages_cpp)

#set_target_properties(laser_publisher PROPERTIES LINK_FLAGS "-lusb-1.0" COMPILE_FLAGS "-lusb-1.0" LINKER_LANGUAGE "CXX")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lusb-1.0")
