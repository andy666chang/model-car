#pragma once

#define SYS_INIT_EARLY 0
#define SYS_INIT_NORMAL 10
#define SYS_INIT_LATE 20

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


#if CONFIG_MULTITHREADING

#define APP_PRIO_HIGH    0
#define APP_PRIO_NORMAL  5
#define APP_PRIO_LOW    10

#define APP_STACK_SIZE 512

#endif
