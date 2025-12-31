/*
 * @Author: andy.chang 
 * @Date: 2024-12-31 14:50:13 
 * @Last Modified by: andy.chang
 * @Last Modified time: 2024-12-31 14:59:13
 */

#pragma once

#define SHELL_END { NULL, NULL, NULL, NULL}

struct shell_t {
    char *func_name;
    char *func_info;
    int (*func)(int argc, char *argv[]);
    struct shell_t *sub_shell ;
};
