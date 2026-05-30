# SPDX-License-Identifier: Apache-2.0

if (BOARD STREQUAL qemu_cortex_m3)
    set(driver_src
        "${CMAKE_CURRENT_SOURCE_DIR}/simulator.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/uart.c"
    )
else()
    file(GLOB driver_src "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
    list(REMOVE_ITEM driver_src "${CMAKE_CURRENT_SOURCE_DIR}/simulator.c")
endif()

target_sources(app PRIVATE ${driver_src})
