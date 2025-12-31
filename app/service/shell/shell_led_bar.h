/*
 * @Author: andy.chang 
 * @Date: 2024-12-31 14:51:58 
 * @Last Modified by: andy.chang
 * @Last Modified time: 2024-12-31 15:11:11
 */

#pragma once

#include <errno.h>
#include "shell.h"

#define SHELL_LED_BAR_CMD "led_bar"
#define SHELL_LED_BAR_INFO "led_bar <r> <g> <b>"
#define SHELL_LED_BAR_FUNC shell_led_bar
#define SHELL_LED_BAR_LIST NULL

int shell_led_bar(int argc, char *argv[]);

#define SHELL_LED_BAR                                                          \
    {SHELL_LED_BAR_CMD, SHELL_LED_BAR_INFO, SHELL_LED_BAR_FUNC,                \
     SHELL_LED_BAR_LIST}
