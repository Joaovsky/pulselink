cmake_minimum_required(VERSION 3.20)
project(pulselink_gateway CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.2)
FetchContent_MakeAvailable(Catch2)

add_library(protocol src/protocol.cpp)
target_include_directories(protocol PUBLIC src)
set_target_properties(protocol PROPERTIES CXX_CLANG_TIDY "clang-tidy")

enable_testing()
add_executable(protocol_tests tests/test_protocol.cpp)
target_link_libraries(protocol_tests PRIVATE protocol Catch2::Catch2WithMain)
add_test(NAME protocol_tests COMMAND protocol_tests)
