cmake_minimum_required(VERSION 3.5)
project (math)

set(CMAKE_CXX_STANDARD 20)

add_library(math_lib SHARED math.cpp)
set_target_properties(math_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "math")
