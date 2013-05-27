cmake_minimum_required(VERSION 2.6)
project(crazyflie)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(crazyflie_VERSION_MAJOR_0)
set(crazyflie_VERSION_MINOR_1)

add_library(cflie
  src/classes/CCrazyRadio.cpp
  src/classes/CCrazyflie.cpp
  src/classes/CCRTPPacket.cpp)

add_executable(crazyflie src/main.cpp)
target_link_libraries(crazyflie cflie usb-1.0)
