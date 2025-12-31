/*
 * @Author: andy.chang 
 * @Date: 2024-12-31 14:51:58 
 * @Last Modified by: andy.chang
 * @Last Modified time: 2024-12-31 15:07:14
 */

#pragma once

#include <errno.h>
#include "shell.h"

#define SHELL_SYS_CMD "sys"
#define SHELL_SYS_INFO "sys func"
#define SHELL_SYS_FUNC NULL
#define SHELL_SYS_LIST _sys_list

#define SHELL_SYS {SHELL_SYS_CMD, SHELL_SYS_INFO, SHELL_SYS_FUNC, SHELL_SYS_LIST}

extern struct shell_t _sys_list[];


