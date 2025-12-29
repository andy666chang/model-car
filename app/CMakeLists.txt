# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(module_car)

target_include_directories(app PRIVATE .)

add_subdirectory(interface)
add_subdirectory(module)
add_subdirectory(service)
target_sources(app PRIVATE main.c)
target_sources(app PRIVATE system.c)
