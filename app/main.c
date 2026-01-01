/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/hwinfo.h>

#include "interface/interface.h"
#include "module/log.h"

#define TAG "main"

int main(void)
{
    uint8_t uuid[12];
    ssize_t len = hwinfo_get_device_id(uuid, sizeof(uuid));

    log_route(k_uptime_get);

    LOG_INF("Board: %s", CONFIG_BOARD);
    LOG_INF("Target: %s", CONFIG_BOARD_TARGET);

    if (len > 0) {
        printf("Device UUID: ");
        for (int i = 0; i < len; i++) {
            printf("%02X", uuid[i]);
        }
        printf("\n");
    } else {
        printf("Failed to get UUID, error: %zd\n", len);
    }

#ifndef CONFIG_MULTITHREADING
    while(1) {

        btn_service_process();
        thro_service_process();
        shell_service_process();

        k_cpu_idle();

    }
#endif

	return 0;
}
