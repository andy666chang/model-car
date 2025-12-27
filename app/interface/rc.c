
#include <stdio.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>
#include "interface/interface.h"

LOG_MODULE_REGISTER(rc, LOG_LEVEL_INF);


static const struct device *pwm1_dev = DEVICE_DT_GET(DT_NODELABEL(pwm1));
static const struct device *pwm2_dev = DEVICE_DT_GET(DT_NODELABEL(pwm2));


static void sg_thro_cb(const struct device *dev, uint32_t channel, 
                      uint32_t period_cycles, uint32_t pulse_cycles, 
                      int status, void *user_data) {
    thro_data_push(pulse_cycles);
}

static void sg_btn_cb(const struct device *dev, uint32_t channel, 
                      uint32_t period_cycles, uint32_t pulse_cycles, 
                      int status, void *user_data) {
    btn_data_push(pulse_cycles);
}

static int rc_init(void) {
    int ret = 0;

    LOG_INF("Initializing RC interface...");

    if (!device_is_ready(pwm1_dev) || !device_is_ready(pwm2_dev)) {
        return 0;
    }


    // Set CH2 (PB3) to Both Edges
    ret = pwm_configure_capture(pwm2_dev, 2, 
        PWM_CAPTURE_TYPE_PULSE | PWM_CAPTURE_MODE_CONTINUOUS, 
        sg_btn_cb, NULL);
    if (ret)
        printf("PWM CH2 init error! ret = %d\n", ret);

    // Set CH3 (PA10) to Both Edges
    ret = pwm_configure_capture(pwm1_dev, 3, 
        PWM_CAPTURE_TYPE_PULSE | PWM_CAPTURE_MODE_CONTINUOUS, 
        sg_thro_cb, NULL);
    if (ret)
        printf("PWM CH3 init error! ret = %d\n", ret);


    ret = pwm_enable_capture(pwm2_dev, 2);
    if (ret)
        printf("PWM CH2 enable error! ret = %d\n", ret);
    ret = pwm_enable_capture(pwm1_dev, 3);
    if (ret)
        printf("PWM CH3 enable error! ret = %d\n", ret);


    return ret;
}

SYS_INIT(rc_init, APPLICATION, 0);
