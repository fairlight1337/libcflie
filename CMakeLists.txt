cmake_minimum_required(VERSION 2.6)
project(cflie)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(${PROJECT_NAME}_VERSION_MAJOR_0)
set(${PROJECT_NAME}_VERSION_MINOR_1)

include_directories("${PROJECT_SOURCE_DIR}/include")

find_library(USB_LIB usb-1.0)

add_library(${PROJECT_NAME}
  src/cflie/CCrazyRadio.cpp
  src/cflie/CCrazyflie.cpp
  src/cflie/CCRTPPacket.cpp
  src/cflie/CTOC.cpp)

add_executable(ex-replugging src/examples/replugging.cpp)
add_executable(ex-simple src/examples/simple.cpp)
add_executable(ex-gui src/examples/gui.cpp)

target_link_libraries(${PROJECT_NAME} ${USB_LIB})
target_link_libraries(ex-replugging ${PROJECT_NAME})
target_link_libraries(ex-simple ${PROJECT_NAME})
target_link_libraries(ex-gui ${PROJECT_NAME} glfw GL GLU)

install(FILES include/cflie/*.h DESTINATION include)
install(FILES lib/* DESTINATION lib)
