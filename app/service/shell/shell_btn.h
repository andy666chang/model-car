/*
 * @Author: andy.chang 
 * @Date: 2024-12-31 14:51:58 
 * @Last Modified by: andy.chang
 * @Last Modified time: 2024-12-31 15:11:49
 */

#pragma once

#include <errno.h>
#include "shell.h"

#define SHELL_BTN_CMD "btn"
#define SHELL_BTN_INFO "btn -p <1...9>"
#define SHELL_BTN_FUNC shell_btn
#define SHELL_BTN_LIST NULL

int shell_btn(int argc, char *argv[]);

#define SHELL_BTN {SHELL_BTN_CMD, SHELL_BTN_INFO, SHELL_BTN_FUNC, SHELL_BTN_LIST}
