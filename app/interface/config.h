/*
 * @Author: andy.chang 
 * @Date: 2024-12-31 10:40:40 
 * @Last Modified by: andy.chang
 * @Last Modified time: 2025-01-02 16:36:41
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct prj_cfg_t {
    uint8_t version;
    
    int8_t dir;
    uint16_t center;
    uint16_t margin;
    uint16_t max;

    bool mode;
    uint8_t bar_idx;
} prj_cfg_t;

extern prj_cfg_t *prj_cfg;

void load_config(void);
void save_config(void);
void dump_config(void);
