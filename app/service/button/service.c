
#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/sys/ring_buffer.h>
#include "service.h"
#include "interface/interface.h"
#include "system.h"

#define BTN_TIMEOUT 750
#define FLASH_TIMEOUT 500
#define CALI_TIMEOUT 1500


#define BTN_SWITCH      1
#define BTN_HIGH_BEAM   2
#define BTN_FALSH       3
#define BTN_LED_SEL     4
#define BTN_CALI        6
#define BTN_MODE        7
#define BTN_DIR         9

#define ON 1
#define OFF 0
#define RC_MAX 2000
#define RC_MIN 1000
#define RC_CENTER 1500

static uint32_t time = 0; // Record time stamp for fast click

RING_BUF_DECLARE(btn_buf, 64);

void btn_data_push(uint16_t data) {
    ring_buf_put(&btn_buf, &data, sizeof(data));
}

uint8_t sw_state = 0;
bool led_stop_blink = false;
static bool beam_state = 0;

static void btn_switch(void) {
    sw_state++;
    sw_state %= 4;
    printf("%s state: %d\n", __func__, sw_state);

    switch (sw_state) {
    case 0:
        led_chasis_set(OFF);
        led_tail_set(0, OFF);
        break;

    case 1:
        led_chasis_set(ON);
        led_tail_set(0, ON);
        break;

    case 2:
        led_chasis_set(ON);
        led_tail_set(0, ON);
        led_head_set(0, ON);
        break;

    case 3:
        led_chasis_set(ON);
        led_tail_set(0, ON);
        led_head_set(0, OFF);
        break;
    
    default:
        break;
    }
}

static void btn_high_beam(void) {
    beam_state = !beam_state;
    printf("%s beam: %d\n", __func__, beam_state);

    if (beam_state) {
        // turn on high beam
        led_head_set(1, ON);
        led_head_set(0, ON);
    } else {
        // turn off high beam
        led_head_set(1, OFF);

        // Keep btn switch status
        if (sw_state == 2)
            led_head_set(0, ON);
        else
            led_head_set(0, OFF);
    }
}

/**
 * @brief 
 * 
 */
void btn_service_process(void) {
    static uint16_t pre_btn = RC_MAX;
    static uint8_t cnt = 0;

    while (!ring_buf_is_empty(&btn_buf)) {
        uint16_t data = 0;
        ring_buf_get(&btn_buf, &data, sizeof(data));

        // printf("Button signal: %d\n", data); // 978 or 2045 in each 15ms
        if (data >= RC_CENTER) {
            data = RC_MAX;
        } else {
            data = RC_MIN;
        }

        // Diff Button state
        if (data != pre_btn) {
            // count timeout
            time = GET_SYS_TIME();
            cnt++;
        }
        
        // record Button state
        pre_btn = data;
    }

    // Remove initial noise
    if (GET_SYS_TIME() <= 2000) {
        cnt = 0;
    }

    // Count timeout
    if (WAIT_TIMEOUT(time, BTN_TIMEOUT) && cnt) {
        LOG_INF("BTN cnt: %d", cnt);
        printf("BTN cnt: %d\n", cnt);
        
        // Send event
        switch (cnt) {
        case BTN_SWITCH: // Switch on/off
            if (system_get_state() == SYSTEM_LED_SELECT) {
                prj_cfg->bar_idx++;
                if (prj_cfg->bar_idx >= led_idx_max()) {
                    prj_cfg->bar_idx = 0;
                }
                thro_led_update(prj_cfg->max);
                printf("Select bar idx: %d\n", prj_cfg->bar_idx);
            } else {
                printf("BTN_SWITCH\n");
                btn_switch();
            }
            break;

        case BTN_HIGH_BEAM: // High beam
            printf("BTN_HIGH_BEAM\n");
            btn_high_beam();
            break;

        case BTN_FALSH: // LED Flash
            printf("BTN_FLASH\n");
            if (led_stop_blink) {
                if (sw_state == 0)
                    led_chasis_set(OFF);
                else 
                    led_chasis_set(ON);

                led_stop_blink = false;
            } else {
                led_stop_blink = true;
            }
            break;
        
        case BTN_LED_SEL:
            printf("BTN_LED_SEL\n");
            if (system_get_state() == SYSTEM_NORMAL) {
                system_set_state(SYSTEM_LED_SELECT);
                thro_led_update(prj_cfg->max);
            } else if (system_get_state() == SYSTEM_LED_SELECT) {
                save_config();
                system_set_state(SYSTEM_NORMAL);
            }
            break;

        case BTN_CALI: // Calibration
            printf("BTN_CALI\n");
            if (system_get_state() == SYSTEM_NORMAL) {
                system_set_state(SYSTEM_CALIBRATION);
            }
            break;

        case BTN_DIR: // Switch direction
            printf("BTN_DIR\n");
            prj_cfg->dir = -prj_cfg->dir;
            printf("dir = %d", prj_cfg->dir);
            save_config();

            // Restart
            k_busy_wait(100*1000);
            sys_reboot(SYS_REBOOT_COLD);
            break;

        case BTN_MODE: // Switch mode
            printf("BTN_MODE\n");
            prj_cfg->mode = !prj_cfg->mode;
            printf("mode = %d\n", prj_cfg->mode);
            save_config();

            if (prj_cfg->mode == 1) {
                led_string_set(ON);
            } else {
                led_string_set(OFF);
            }
            break;

        default:
            break;
        }

        cnt = 0;
    }
    
    return;
}
