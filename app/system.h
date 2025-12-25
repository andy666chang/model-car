#pragma once

enum sys_state_t {
    SYSTEM_NORMAL = 0,
    SYSTEM_CALIBRATION,
    SYSTEM_LED_SELECT,

    SYSTEM_MAX,
};

/**
 * @brief Set the system state
 * 
 * @param state The new state to set
 */
void system_set_state(enum sys_state_t state);

/**
 * @brief Get the current system state
 * 
 * @return sys_state_t The current system state
 */
enum sys_state_t system_get_state(void);
