cmake_minimum_required(VERSION 2.6)
project(crazyflie)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(crazyflie_VERSION_MAJOR_0)
set(crazyflie_VERSION_MINOR_1)

include_directories("${PROJECT_SOURCE_DIR}/include")

find_library(USB_LIB usb-1.0)

add_library(cflie
  src/cflie/CCrazyRadio.cpp
  src/cflie/CCrazyflie.cpp
  src/cflie/CCRTPPacket.cpp)

add_executable(test src/main.cpp)

target_link_libraries(cflie ${USB_LIB})
target_link_libraries(test cflie)
