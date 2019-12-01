cmake_minimum_required(VERSION 3.10)

project(ogg_decode_example)

set(CMAKE_CXX_STANDARD 14)

add_executable(decode_example example/decode_example.cpp example/AudioFile.cpp)
target_include_directories(decode_example
        PRIVATE ${CMAKE_SOURCE_DIR})