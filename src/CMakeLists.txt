cmake_minimum_required(VERSION 3.10)

include_directories(./)
file(GLOB_RECURSE SRCS *.cpp)
file(GLOB_RECURSE HDRS *.h)
add_executable(mlua main.cpp ${SRCS})
target_compile_features(mlua PRIVATE cxx_std_17)

