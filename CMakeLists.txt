cmake_minimum_required(VERSION 2.8.3)
project(simulation)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  nav_msgs
  roscpp
  rospy
  tf
)

catkin_package()

include_directories(${catkin_INCLUDE_DIRS})

add_executable(modelsim src/modelsim.cpp)
target_link_libraries(modelsim ${catkin_LIBRARIES})

catkin_install_python(
  PROGRAMS
  src/keyboard_vel_controller.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)