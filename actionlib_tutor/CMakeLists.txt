cmake_minimum_required(VERSION 3.0.2)
project(actionlib_tutor)

find_package(catkin REQUIRED COMPONENTS
  actionlib
  actionlib_msgs
  message_generation
  roscpp
  rospy
  std_msgs
)

# Generate actions in the 'action' folder
add_action_files(
  FILES
  Fibonacci.action
)
# Generate added messages and services with any dependencies listed here
generate_messages(
  DEPENDENCIES
  actionlib_msgs
  std_msgs
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES actionlib_tutor
 CATKIN_DEPENDS actionlib actionlib_msgs message_generation roscpp rospy std_msgs
#  DEPENDS system_lib
)

include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)

add_executable(fibonacci_server src/fibonacci_server.cpp)

target_link_libraries(
  fibonacci_server
  ${catkin_LIBRARIES}
)

add_dependencies(
  fibonacci_server
  ${actionlib_tutor_EXPORTED_TARGETS}
)

add_executable(fibonacci_client src/fibonacci_client.cpp)

target_link_libraries(
  fibonacci_client
  ${catkin_LIBRARIES}
)

add_dependencies(
  fibonacci_client
  ${actionlib_tutor_EXPORTED_TARGETS}
)
