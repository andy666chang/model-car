
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



void led_head_set(uint8_t id, bool en);
void led_tail_set(uint8_t id, bool en);
void led_chasis_set(bool en);
void led_fire_set(bool en);
void led_string_set(bool en);
void thro_led_update(int16_t thro);
void thro_led_brake(void);
void thro_led_set(uint8_t r, uint8_t g, uint8_t b);
uint8_t led_idx_max(void);

void btn_data_push(uint16_t data);
