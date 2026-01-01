
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <module/shell.h>
#include <module/log.h>

#include "service/button/service.h"

#define TAG "SHELL-BTN"

static int shell_btn(int argc, char *argv[]) {
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

#include <zephyr/kernel.h>

STRUCT_SECTION_ITERABLE(shell_t, btn) = {
    .name = "btn",
    .info = "btn -p <1...9>",
    .func = shell_btn,
    .sub = NULL,
};
