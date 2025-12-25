/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/hwinfo.h>
#include <zephyr/logging/log.h>
#include "interface/interface.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    uint8_t uuid[12];
    ssize_t len = hwinfo_get_device_id(uuid, sizeof(uuid));

    #if CONFIG_LOG
    LOG_INF("Board: %s", CONFIG_BOARD);
    LOG_INF("Target: %s", CONFIG_BOARD_TARGET);
    #else
    printf("Board: %s\n", CONFIG_BOARD);
    printf("Target: %s\n", CONFIG_BOARD_TARGET);
    #endif

    if (len > 0) {
        printf("Device UUID: ");
        for (int i = 0; i < len; i++) {
            printf("%02X", uuid[i]);
        }
        printf("\n");
    } else {
        printf("Failed to get UUID, error: %zd\n", len);
    }


    while(1) {

        btn_service_process();
        thro_service_process();

    }

	return 0;
}
