
#include "system.h"
#include "module/log.h"
#include "interface/interface.h"

#include <zephyr/sys/reboot.h>
#include <zephyr/sys/ring_buffer.h>

#define TAG "THRO"

#define THRO_SHORT_TIMEOUT 300
#define THRO_LONG_TIMEOUT  1000
#define THRO_FIRE_TIMEOUT  50
#define THRO_WAIT_TIMEOUT  3000
#define BLINK_TIMEOUT      500
#define THRO_CALI_STEP1    2000
#define THRO_CALI_STEP2    (2 * THRO_CALI_STEP1)
#define THRO_CALI_TIMEOUT  (5 * THRO_CALI_STEP1)

enum {
    THRO_INPUT = 0,
    THRO_SHORT,
    THRO_LONG,
    THRO_FIRE,
    THRO_WAIT,
    THRO_BRAKE,
    THRO_CALI,
};

#define ON 1
#define OFF 0

// int16_t centor = 1500; // 1000 ~ 2000 us
// static const uint16_t margin = 20; // 20us
extern uint8_t sw_state;
extern bool led_stop_blink;

static uint16_t event_cap = 0;
static uint32_t short_timeout;
static uint32_t long_timeout;
static uint32_t fire_timeout;
static uint32_t wait_timeout;
static uint32_t cali_time; // Record time stamp for calibration
static int16_t pre_thro = 0;
static int16_t thro = 0;
static uint16_t data = 0;

RING_BUF_DECLARE(thro_buf, 64);

void thro_data_push(uint16_t data) {
    ring_buf_put(&thro_buf, &data, sizeof(data));
}

/**
 * @brief 
 * 
 */
static inline void thro_short(void) {
    // turn on tail led
    led_tail_set(0, ON);
    led_tail_set(1, ON);

    if (WAIT_TIMEOUT(short_timeout, THRO_SHORT_TIMEOUT)) {
        // turn off tail led
        if (sw_state == 0)
            led_tail_set(0, OFF);
        led_tail_set(1, OFF);

        // Clear event
        event_cap &= ~(BIT(THRO_SHORT));
    }
}

/**
 * @brief 
 * 
 */
static inline void thro_long(void) {
    // turn on tail led
    led_tail_set(0, ON);
    led_tail_set(1, ON);

    if (WAIT_TIMEOUT(long_timeout, THRO_LONG_TIMEOUT)) {
        // turn off tail led
        if (sw_state == 0)
            led_tail_set(0, OFF);
        led_tail_set(1, OFF);

        // Clear event
        event_cap &= ~(BIT(THRO_LONG));
    }
}

/**
 * @brief 
 * 
 */
static inline void thro_fire(void) {
    uint32_t duration = GET_SYS_TIME() - fire_timeout;

    if (duration > 3 * THRO_FIRE_TIMEOUT) {
        // turn off fire led
        led_fire_set(OFF);

        // Clear event
        event_cap &= ~(BIT(THRO_FIRE));
    } else if (duration > 2 * THRO_FIRE_TIMEOUT) {
        // turn off fire led
        led_fire_set(ON);
    } else if (duration > 1 * THRO_FIRE_TIMEOUT) {
        // turn off fire led
        led_fire_set(OFF);
    } else if (duration < THRO_FIRE_TIMEOUT) {
        // turn on fire led
        led_fire_set(ON);
    }
}

/**
 * @brief 
 * 
 */
static inline void thro_blink_wait(void) {
    static uint32_t blink_time = 0;
    static bool led_state = 0;

    if ((abs(thro) > prj_cfg->margin) || (led_stop_blink == false)) {
        // Cancel wait
        event_cap &= ~BIT(THRO_WAIT);
        led_chasis_set(OFF);
        led_state = OFF;

        // Recover led btn status
        if (led_stop_blink == false) {
            if (sw_state == 0)
                led_chasis_set(OFF);
            else
                led_chasis_set(ON);
        }
    } else if (WAIT_TIMEOUT(wait_timeout, THRO_WAIT_TIMEOUT)) {
        // led blink
        if (WAIT_TIMEOUT(blink_time, BLINK_TIMEOUT)) {
            led_state = !led_state;
            led_chasis_set(led_state);
            blink_time = GET_SYS_TIME();
        }
    }
}

/**
 * @brief 
 * 
 */
static inline void thro_brake(void) {
    // turn on tail led
    led_tail_set(0, ON);
    led_tail_set(1, ON);

    if ((abs(thro) < prj_cfg->margin) || (thro > prj_cfg->margin)) {
        // turn off tail led
        if (sw_state == 0)
            led_tail_set(0, OFF);
        led_tail_set(1, OFF);

        // Clear event
        event_cap &= ~(BIT(THRO_BRAKE));
    }
}

/**
 * @brief 
 * 
 */
static inline void thro_cali(void) {
    //  Capture center value
    if ((GET_SYS_TIME() - cali_time) < THRO_CALI_STEP1) {
        //  Capture center value
        prj_cfg->center = data;

        led_tail_set(0, ON);
        led_tail_set(1, ON);
    } else if ((GET_SYS_TIME() - cali_time) < THRO_CALI_STEP2) {
        //  Capture max value
        prj_cfg->max = data;

        led_fire_set(ON);
    } else {
        LOG_INF("center = %d", prj_cfg->center);
        LOG_INF("max = %d", prj_cfg->max);
        save_config();

        // Exit calibration mode
        system_set_state(SYSTEM_NORMAL);
        LOG_INF("Exit calibration mode");

        // Clear event
        event_cap &= ~(BIT(THRO_CALI));

        // Restart
        k_busy_wait(100*1000);
        sys_reboot(SYS_REBOOT_COLD);
    }
}

/**
 * @brief 
 * 
 */
void thro_service_process(void) {

    while (!ring_buf_is_empty(&thro_buf)) {
        ring_buf_get(&thro_buf, &data, sizeof(data));

        // Check calibration
        if (system_get_state() == SYSTEM_CALIBRATION) {
            if ((event_cap & BIT(THRO_CALI)) == 0) {
                event_cap |= BIT(THRO_CALI);
                cali_time = GET_SYS_TIME();
            }
            break;
        } else if (system_get_state() == SYSTEM_LED_SELECT) {
            break;
        }

        thro = data - prj_cfg->center;
        thro *= prj_cfg->dir;

        LOG_DBG("Throttle signal: %5d", thro); // 1500 +- 544 in each 15ms

        // Check short 
        //   throttle decrease
        //   throttle change larger than margin
        //   throttle larger than center
        if ((abs(thro) < abs(pre_thro)) &&
            (abs(thro-pre_thro) > prj_cfg->margin) &&
            (thro > prj_cfg->margin)) {
            LOG_INF("THRO_SHORT");
            event_cap |= BIT(THRO_SHORT);
            short_timeout = GET_SYS_TIME();
        }

        // Check brake
        //   bypass already brake
        //   throttle is negative
        if (!(event_cap & BIT(THRO_BRAKE)) &&
            (abs(thro) > prj_cfg->margin) &&
            (thro < 0)) {
            LOG_INF("THRO_BRAKE");
            event_cap |= BIT(THRO_BRAKE);
        }

        // Check long 
        // if ((abs(thro) < abs(pre_thro)) &&
        //     (abs(thro-pre_thro) > prj_cfg->margin) &&
        //     (abs(thro) < prj_cfg->margin)) {
        //     LOG_INF("THRO_LONG");
        //     event_cap |= BIT(THRO_LONG);
        //     long_timeout = GET_SYS_TIME();
        // }

        // Check fire
        //   previous throttle larger than margin
        //   throttle in center/negative
        if ((pre_thro > prj_cfg->margin) &&
            (thro < prj_cfg->margin)) {
            LOG_INF("THRO_FIRE");
            event_cap |= BIT(THRO_FIRE);
            fire_timeout = GET_SYS_TIME();
        }

        // Check stop blink led
        if (led_stop_blink && ((event_cap & BIT(THRO_WAIT)) == 0)) {
            if (abs(thro) < prj_cfg->margin) {
                wait_timeout = GET_SYS_TIME();
                event_cap |= BIT(THRO_WAIT);
            }
        }

        // Record thro state
        pre_thro = thro;

        event_cap |= BIT(THRO_INPUT);
    }

    /* Check throttle event */
    // short 
    if (event_cap & BIT(THRO_SHORT)) {
        thro_short();
    }

    // long
    // if (event_cap & BIT(THRO_LONG)) {
    //     thro_long();
    // }

    // fire
    if (event_cap & BIT(THRO_FIRE)) {
        thro_fire();
    }

    // wait
    if (event_cap & BIT(THRO_WAIT)) {
        thro_blink_wait();
    }

    // Brake
    if (event_cap & BIT(THRO_BRAKE)) {
        thro_brake();
    }

    // calibration
    if ((event_cap & BIT(THRO_CALI)) &&
        (GET_SYS_TIME() - cali_time) < THRO_CALI_TIMEOUT) {
        thro_cali();
    }

    // Update throttle led bar
    if (prj_cfg->mode == 1 && (event_cap & BIT(THRO_INPUT))) {
        if (event_cap & BIT(THRO_BRAKE)) {
            thro_led_brake();
        } else {
            thro_led_update(thro);
        }
    }

    event_cap &= ~(BIT(THRO_INPUT));
    return;
}
