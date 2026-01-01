# SPDX-License-Identifier: Apache-2.0

if(NOT CONFIG_LOG)
    target_sources(app PRIVATE log.c)
endif()

if(NOT CONFIG_SHELL)
    target_sources(app PRIVATE shell.c)
    zephyr_linker_sources(SECTIONS shell.ld)
endif()
