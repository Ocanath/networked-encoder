if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.22)
    project(networked_encoder C CXX)
endif()

add_library(encoder_controller STATIC
    encoder.cpp
    encoder_serial_callbacks.cpp
)

if(NOT TARGET dartt_protocol)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/dartt-protocol)
endif()

if(NOT TARGET cobs)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/byte-stuffing)
endif()

if(NOT TARGET serial)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/serial-cross-platform)
endif()

target_include_directories(encoder_controller PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../embedded/encoder-rs485/Core/Inc
)

target_link_libraries(encoder_controller PUBLIC
    dartt_protocol
    serial
    cobs
)
