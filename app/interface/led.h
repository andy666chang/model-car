
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
    LED_HEAD_0 = 0,
    LED_HEAD_1,
    LED_TAIL_0,
    LED_TAIL_1,
    LED_CHASIS,
    LED_FIRE,

    LED_PIN_MAX,
} led_t;

void led_set(led_t id, bool en);

