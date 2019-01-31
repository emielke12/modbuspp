cmake_minimum_required(VERSION 2.8.3)
project(modbuspp)

add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
)

catkin_package(
  INCLUDE_DIRS
    include
  LIBRARIES
    modbuspp
#  CATKIN_DEPENDS other_catkin_pkg
#  DEPENDS
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(include ${catkin_INCLUDE_DIRS})

## Declare a C++ library
add_library(modbuspp
   src/modbus.cpp
 )

## Declare a C++ executable
add_executable(modbus_example
  src/example.cpp
)

## Add cmake target dependencies of the executable
## same as for the library above
# add_dependencies(ati_netft_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Specify libraries to link a library or executable target against
target_link_libraries(modbus_example
  ${catkin_LIBRARIES}
  modbuspp
)
