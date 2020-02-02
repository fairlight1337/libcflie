cmake_minimum_required(VERSION 2.6)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

project(cflie)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(${PROJECT_NAME}_VERSION_MAJOR_0)
set(${PROJECT_NAME}_VERSION_MINOR_1)

include_directories("${PROJECT_SOURCE_DIR}/include")

link_directories("/usr/lib/x86_64-linux-gnu/")

find_library(GLFW_LIB glfw)
if(${GLFW_LIB} STREQUAL "GLFW_LIB-NOTFOUND")
  message(STATUS "The GLFW libs appear to not be installed.")
  message(FATAL_ERROR "On Ubuntu, do: sudo apt-get install libglfw-dev libglfw2")
endif()

find_library(USB_LIB usb-1.0)
if(${USB_LIB} STREQUAL "USB_LIB-NOTFOUND")
  message(STATUS "The USB libs appear to not be installed.")
  message(FATAL_ERROR "On Ubuntu, do: sudo apt-get install libusb-1.0-0 libusb-1.0-0-dev")
endif()


### Libraries ###

add_library(${PROJECT_NAME}
  src/cflie/CCrazyRadio.cpp
  src/cflie/CCrazyflie.cpp
  src/cflie/CCRTPPacket.cpp
  src/cflie/CTOC.cpp)


### Executables ###

add_executable(ex-replugging src/examples/replugging.cpp)
add_executable(ex-simple src/examples/simple.cpp)
add_executable(ex-gui src/examples/gui.cpp)


### Linking ###

target_link_libraries(${PROJECT_NAME} ${USB_LIB})
target_link_libraries(ex-replugging ${PROJECT_NAME})
target_link_libraries(ex-simple ${PROJECT_NAME})
target_link_libraries(ex-gui ${PROJECT_NAME} ${GLFW_LIB} GL GLU)

# ARM needs librt linked in
if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm")
  target_link_libraries(${PROJECT_NAME} rt)
  target_link_libraries(ex-replugging rt)
  target_link_libraries(ex-simple rt)
  target_link_libraries(ex-gui rt)
endif()


### Install ###

install(
  DIRECTORY include/cflie
  DESTINATION include
  FILES_MATCHING
  PATTERN "*.h"
  PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install(
  TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  COMPONENT library)
