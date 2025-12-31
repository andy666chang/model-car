/*
 * @Author: andy.chang 
 * @Date: 2024-12-31 14:51:58 
 * @Last Modified by: andy.chang
 * @Last Modified time: 2024-12-31 15:11:11
 */

#pragma once

#include <errno.h>
#include "shell.h"

#define SHELL_LED_CMD "led"
#define SHELL_LED_INFO "led -i <0...5> -l <0...1>"
#define SHELL_LED_FUNC shell_led
#define SHELL_LED_LIST NULL

int shell_led(int argc, char *argv[]);

#define SHELL_LED {SHELL_LED_CMD, SHELL_LED_INFO, SHELL_LED_FUNC, SHELL_LED_LIST}
