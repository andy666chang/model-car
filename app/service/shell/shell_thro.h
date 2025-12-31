/*
 * @Author: andy.chang 
 * @Date: 2024-12-31 14:51:58 
 * @Last Modified by: andy.chang
 * @Last Modified time: 2024-12-31 15:20:44
 */

#pragma once

#include <errno.h>
#include "shell.h"

#define SHELL_THRO_CMD "thro"
#define SHELL_THRO_INFO "thro -v <1000...2000>"
#define SHELL_THRO_FUNC shell_thro
#define SHELL_THRO_LIST NULL

int shell_thro(int argc, char *argv[]);

#define SHELL_THRO {SHELL_THRO_CMD, SHELL_THRO_INFO, SHELL_THRO_FUNC, SHELL_THRO_LIST}
