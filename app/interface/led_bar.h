
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void led_string_set(bool en);
void thro_led_update(int16_t thro);
void thro_led_brake(void);
void thro_led_set(uint8_t r, uint8_t g, uint8_t b);
uint8_t led_idx_max(void);
