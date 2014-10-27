cmake_minimum_required(VERSION 2.6)
project(cflie)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(${PROJECT_NAME}_VERSION_MAJOR_0)
set(${PROJECT_NAME}_VERSION_MINOR_1)

include_directories("${PROJECT_SOURCE_DIR}/include")

find_library(USB_LIB usb-1.0)
if(${USB_LIB} STREQUAL "USB_LIB-NOTFOUND")
  message(STATUS "The USB libs appear to not be installed.")
  message(FATAL_ERROR "On Ubuntu, do: sudo apt-get install libusb-1.0-0 libusb-1.0-0-dev")
endif()

add_library(${PROJECT_NAME}
  src/cflie/CCrazyRadio.cpp
  src/cflie/CCrazyflie.cpp
  src/cflie/CTOC.cpp)

add_executable(ex-replugging src/examples/replugging.cpp)
add_executable(ex-simple src/examples/simple.cpp)

target_link_libraries(${PROJECT_NAME} ${USB_LIB})
target_link_libraries(ex-replugging ${PROJECT_NAME})
target_link_libraries(ex-simple ${PROJECT_NAME})

install(DIRECTORY include/cflie
  DESTINATION include
  FILES_MATCHING
  PATTERN "*.h"
  PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install(
  TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  COMPONENT library
)

if(CMAKE_COMPILER_IS_GNUCXX)
  # enable c++11 and some other reasonable flags for gcc
  set(MY_COMPILE_FLAGS "-std=gnu++11 -pthread -Wall -Wextra -Wno-strict-aliasing -fvisibility-inlines-hidden")
  set(MY_LINK_FLAGS "-Wl,--as-needed ${MY_COMPILE_FLAGS}")
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
    COMPILE_FLAGS "${MY_COMPILE_FLAGS}"
    LINK_FLAGS "${MY_LINK_FLAGS}"
    LINK_FLAGS_RELEASE "-s"
  )
  SET_TARGET_PROPERTIES(ex-replugging PROPERTIES
    COMPILE_FLAGS "${MY_COMPILE_FLAGS}"
    LINK_FLAGS "${MY_LINK_FLAGS}"
    LINK_FLAGS_RELEASE "-s"
  )
  SET_TARGET_PROPERTIES(ex-simple PROPERTIES
    COMPILE_FLAGS "${MY_COMPILE_FLAGS}"
    LINK_FLAGS "${MY_LINK_FLAGS}"
    LINK_FLAGS_RELEASE "-s"
  )
endif()

find_library(GLFW_LIB glfw)
if(${GLFW_LIB} STREQUAL "GLFW_LIB-NOTFOUND")
  message(STATUS "The GLFW libs appear to not be installed.")
  message(STATUS "On Ubuntu, do: sudo apt-get install libglfw-dev libglfw2")
  message(STATUS "The GUI-example (ex-gui) will NOT be build!")
else()
  add_executable(ex-gui src/examples/gui.cpp)
  target_link_libraries(ex-gui ${PROJECT_NAME} ${GLFW_LIB} GL GLU)
  if(CMAKE_COMPILER_IS_GNUCXX)
    SET_TARGET_PROPERTIES(ex-gui PROPERTIES
      COMPILE_FLAGS "${MY_COMPILE_FLAGS}"
      LINK_FLAGS "${MY_LINK_FLAGS}"
      LINK_FLAGS_RELEASE "-s"
    )
  endif()
endif()
