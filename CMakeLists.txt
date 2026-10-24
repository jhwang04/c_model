# Configuration, run once
# cmake -B build

# Execute
# cmake --build build

cmake_minimum_required(VERSION 3.15)
project(c_model LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(run-model
  src/main.cc
)

target_include_directories(run-model
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
