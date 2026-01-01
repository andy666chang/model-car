/*
 * @Author: andy.chang 
 * @Date: 2024-08-03 01:00:49 
 * @Last Modified by: andy.chang
 * @Last Modified time: 2024-08-03 01:04:17
 */

#ifndef _SHELL_H_
#define _SHELL_H_

#include <stdio.h>
#include <stdint.h>

#define SHELL_END { NULL, NULL, NULL, NULL}

struct shell_t {
    char *name;
    char *info;
    int (*func)(int argc, char *argv[]);
    struct shell_t *sub;
};

/**
 * @brief 
 * 
 * @param data 
 * @param len 
 */
void shell_process(uint8_t *data, uint16_t len);

#endif // _SHELL_H_
