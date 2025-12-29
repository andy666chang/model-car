# SPDX-License-Identifier: Apache-2.0

if(NOT CONFIG_LOG)
    target_sources(app PRIVATE log.c)
endif()

