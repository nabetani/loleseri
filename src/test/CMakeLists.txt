cmake_minimum_required(VERSION 3.10)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "^Windows$")
else()
set(CMAKE_CXX_FLAGS 
    "-std=c++11 -Wall -Wcast-align -Wconversion -Wold-style-cast -Wwrite-strings ")
endif()

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

file(GLOB testers *.cpp)
include_directories(../lib)


add_executable(loleseri_gt ${testers})
target_link_libraries(loleseri_gt gtest_main)
add_test(NAME loleseri_gt_test COMMAND loleseri_gt)