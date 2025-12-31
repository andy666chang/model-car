/*
 * @Author: andy.chang 
 * @Date: 2024-12-31 15:01:03 
 * @Last Modified by: andy.chang
 * @Last Modified time: 2024-12-31 15:20:56
 */

#include "shell_thro.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "service/throttle/service.h"

#include "module/log.h"

#define TAG "SHELL-THRO"

int shell_thro(int argc, char *argv[]) {
    // for (size_t i = 0; i < argc; i++) {
    //     LOGI(TAG, " led sub: %s", argv[i]);
    // }

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
