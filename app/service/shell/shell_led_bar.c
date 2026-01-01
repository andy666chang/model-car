
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <module/shell.h>
#include <module/log.h>

#include "interface/interface.h"

#define TAG "SHELL-LED_BAR"

static int shell_led_bar(int argc, char *argv[]) {

    if (argc < 3) {
        LOGE(TAG, "Invalid args, need 3 args for r,g,b");
        return -EINVAL;
    }

    uint8_t r = 0, g = 0, b = 0;
    r = strtol(argv[0], NULL, 0);
    g = strtol(argv[1], NULL, 0);
    b = strtol(argv[2], NULL, 0);

    LOGI(TAG, "Set led bar color: r = %d, g = %d, b = %d", r, g, b);
    thro_led_set(r, g, b);

    return 0;
}

#include <zephyr/kernel.h>

STRUCT_SECTION_ITERABLE(shell_t, led_bar) = {
    .name = "led_bar",
    .info = "led_bar <r> <g> <b>",
    .func = shell_led_bar,
    .sub = NULL,
};
