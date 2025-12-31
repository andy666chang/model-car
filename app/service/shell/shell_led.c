/*
 * @Author: andy.chang 
 * @Date: 2024-12-31 15:01:03 
 * @Last Modified by: andy.chang
 * @Last Modified time: 2024-12-31 15:17:47
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "shell_led.h"
#include "interface/interface.h"

#include "module/log.h"

#define TAG "SHELL-LED"


int shell_led(int argc, char *argv[]) {
    // for (size_t i = 0; i < argc; i++) {
    //     LOGI(TAG, " led sub: %s", argv[i]);
    // }

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
