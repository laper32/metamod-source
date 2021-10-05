project(MetaMod-Source)
cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

include(configure.cmake)

include(cmake/ConfigureMetaMod.cmake)

add_subdirectory(core)
