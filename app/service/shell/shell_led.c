
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <module/shell.h>
#include <module/log.h>

#include "interface/interface.h"

#define TAG "SHELL-LED"

static int shell_led(int argc, char *argv[]) {
    int idx = 99, level = 99;

    for (int i = 0; i < argc; i++) {
        if ( !strcmp(argv[i], "-i") && (i+1) < argc) {
            idx = strtol(argv[i+1], NULL, 10);
            continue;
        }

        if ( !strcmp(argv[i], "-l") && (i+1) < argc) {
            level = (bool)strtol(argv[i+1], NULL, 10);
            continue;
        }
    }

    LOGI(TAG,"idx: %d, level: %d", idx, level);

    switch (idx) {
        case 0:
            LOGI(TAG, "HEAD_0_Pin level: %d", level);
            led_head_set(0, level);
            break;
        
        case 1:
            LOGI(TAG, "HEAD_1_Pin level: %d", level);
            led_head_set(1, level);
            break;

        case 2:
            LOGI(TAG, "CHASIS_Pin level: %d", level);
            led_chasis_set(level);
            break;

        case 3:
            LOGI(TAG, "TAIL_0_Pin level: %d", level);
            led_tail_set(0, level);
            break;

        case 4:
            LOGI(TAG, "TAIL_1_Pin level: %d", level);
            led_tail_set(1, level);
            break;

        case 5:
            LOGI(TAG, "FIRE_Pin level: %d", level);
            led_fire_set(level);
            break;
        
        default:
            LOGE(TAG,"Unknow LED: %d", idx);
            break;
    }

    return 0;
}

#include <zephyr/kernel.h>

STRUCT_SECTION_ITERABLE(shell_t, led) = {
    .name = "led",
    .info = "led -i <0...5> -l <0...1>",
    .func = shell_led,
    .sub = NULL,
};
