# SPDX-License-Identifier: Apache-2.0

FILE(GLOB interface_src *.c)
target_sources(app PRIVATE ${interface_src})
