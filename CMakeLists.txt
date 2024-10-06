cmake_minimum_required(VERSION 3.10)
project(nest VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(src)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_library(libnest STATIC ${SOURCES})

add_library(libnest_shared SHARED ${SOURCES})

set_target_properties(libnest PROPERTIES OUTPUT_NAME "nest")
set_target_properties(libnest_shared PROPERTIES OUTPUT_NAME "nest")

install(TARGETS libnest libnest_shared DESTINATION lib)

install(DIRECTORY include/ DESTINATION include/nest/include FILES_MATCHING PATTERN "*.h")
