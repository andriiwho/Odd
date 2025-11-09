cmake_minimum_required(VERSION 3.30)
project(Odd)

include(Odd/CMake/Odd.cmake)

set(ODD_THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/Odd/ThirdParty CACHE STRING "")

add_subdirectory(Odd/Src)
add_subdirectory(Even/Src)