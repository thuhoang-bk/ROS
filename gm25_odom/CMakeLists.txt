cmake_minimum_required(VERSION 3.0.2)
project(gm25_odom)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  tf
  nav_msgs
  my_msgs
)

generate_messages(DEPENDENCIES my_msgs)

catkin_package(
#  CATKIN_DEPENDS roscpp rospy tf nav_msgs my_msgs
)
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_executable(odom_publisher src/odom_publisher.cpp)
target_link_libraries(odom_publisher ${catkin_LIBRARIES})
add_dependencies(odom_publisher gm25_odom_generate_messages_cpp)
