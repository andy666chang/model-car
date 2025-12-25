
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "system.h"

static volatile enum sys_state_t sys_state = SYSTEM_NORMAL;

enum sys_state_t system_get_state(void) {
    return sys_state;
}

void system_set_state(enum sys_state_t state) {
    sys_state = state;
}
