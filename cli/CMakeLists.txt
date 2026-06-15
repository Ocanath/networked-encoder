cmake_minimum_required(VERSION 3.22)
project(encoder_cli C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encoder API is the authoritative source for shared deps (dartt_protocol, cobs, serial).
# Add it first so its submodule copies win; bootloader guards will skip re-adding them.
if(NOT TARGET encoder_controller)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../api encoder_api_build)
endif()

if(NOT TARGET dartt_flasher_lib)
    add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/../network-encoder-firmware/encoder-bootloader/dartt_bootloader
        dartt_bl_build
    )
endif()

set(DARTT_BL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../network-encoder-firmware/encoder-bootloader/dartt_bootloader)

add_custom_target(generate_version ALL
    COMMAND ruby ${DARTT_BL_DIR}/scripts/run_get_version.rb
    WORKING_DIRECTORY ${DARTT_BL_DIR}/scripts
    COMMENT "Generating version.h"
)

add_executable(encoder-cli main.cpp args.cpp)

add_dependencies(encoder-cli generate_version)

target_link_libraries(encoder-cli PRIVATE
    encoder_controller
    dartt_flasher_lib
)
