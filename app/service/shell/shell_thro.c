
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <module/shell.h>
#include <module/log.h>

#include "service/throttle/service.h"

#define TAG "SHELL-THRO"

static int shell_thro(int argc, char *argv[]) {
    int thro = 0;

    for (int i = 0; i < argc; i++) {
        if ( !strcmp(argv[i], "-v") && (i+1) < argc) {
            thro = strtol(argv[i+1], NULL, 10);
        }
    }

    LOGI(TAG,"thro: %d", thro);
    thro_data_push(thro);

    return 0;
}

#include <zephyr/kernel.h>

STRUCT_SECTION_ITERABLE(shell_t, thro) = {
    .name = "thro",
    .info = "thro -v <1000...2000>",
    .func = shell_thro,
    .sub = NULL,
};
