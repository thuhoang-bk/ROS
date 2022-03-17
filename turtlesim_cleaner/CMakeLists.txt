cmake_minimum_required(VERSION 3.0.2)
project(turtlesim_cleaner)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  message_generation
  geometry_msgs
)

#------Declare ROS messages and services---

#------Generate added messages and services---
generate_messages(DEPENDENCIES std_msgs geometry_msgs)

#------dependencies------
catkin_package(CATKIN_DEPENDS roscpp rospy std_msgs geometry_msgs message_runtime)
include_directories(${catkin_INCLUDE_DIRS})

#------executables-------

add_executable(turtlesim_cleaner_node src/turtlesim_cleaner_node.cpp)
target_link_libraries(turtlesim_cleaner_node ${catkin_LIBRARIES})
add_dependencies(turtlesim_cleaner_node turtlesim_cleaner_gencpp)

#---pycode---
