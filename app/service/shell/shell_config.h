/*
 * @Author: andy.chang 
 * @Date: 2024-12-31 14:51:58 
 * @Last Modified by: andy.chang
 * @Last Modified time: 2024-12-31 15:59:35
 */

#pragma once

#include <errno.h>
#include "shell.h"

#define SHELL_CFG_CMD "config"
#define SHELL_CFG_INFO "config func"
#define SHELL_CFG_FUNC NULL
#define SHELL_CFG_LIST _cfg_list

#define SHELL_CFG {SHELL_CFG_CMD, SHELL_CFG_INFO, SHELL_CFG_FUNC, SHELL_CFG_LIST}

extern struct shell_t _cfg_list[];


