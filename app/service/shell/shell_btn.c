/*
 * @Author: andy.chang 
 * @Date: 2024-12-31 15:01:03 
 * @Last Modified by: andy.chang
 * @Last Modified time: 2025-01-02 17:22:09
 */

#include "shell_btn.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "service/button/service.h"

#include "module/log.h"

#define TAG "SHELL-BTN"

int shell_btn(int argc, char *argv[]) {
    // for (size_t i = 0; i < argc; i++) {
    //     LOGI(TAG, " led sub: %s", argv[i]);
    // }

    int p = 0;

    for (int i = 0; i < argc; i++) {
        if ( !strcmp(argv[i], "-p") && (i+1) < argc) {
            p = strtol(argv[i+1], NULL, 10);
        }
    }

    LOGI(TAG,"press: %d",p );

    for (int i = 0; i < p; i++){
        static uint16_t btn = 2000;
        
        btn_data_push(btn);

        btn = (btn == 2000)? 1000:2000;
    }

    return 0;
}
