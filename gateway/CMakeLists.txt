cmake_minimum_required(VERSION 3.20)
project(pulselink_gateway CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Threads REQUIRED)

include(FetchContent)
FetchContent_Declare(Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.5.2)
FetchContent_MakeAvailable(Catch2)

add_library(core src/protocol.cpp src/processor.cpp)
target_include_directories(core PUBLIC src)
set_target_properties(core PROPERTIES CXX_CLANG_TIDY "clang-tidy")

add_executable(gateway src/main.cpp src/can_reader.cpp)
target_link_libraries(gateway PRIVATE core Threads::Threads)
set_target_properties(gateway PROPERTIES CXX_CLANG_TIDY "clang-tidy")

enable_testing()
add_executable(protocol_tests tests/test_protocol.cpp)
target_link_libraries(protocol_tests PRIVATE core Catch2::Catch2WithMain)
add_test(NAME protocol_tests COMMAND protocol_tests)

add_executable(processor_tests tests/test_processor.cpp)
target_link_libraries(processor_tests PRIVATE core Catch2::Catch2WithMain)
add_test(NAME processor_tests COMMAND processor_tests)