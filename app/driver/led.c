
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <interface/interface.h>
#include <interface/led.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#include <system.h>
#include "module/log.h"

#define TAG "LED"

static const struct led_pin_t {
    struct gpio_dt_spec dt;
    uint32_t config;
} led_pins[] = {
    [LED_HEAD_0] = {
        .dt = GPIO_DT_SPEC_GET(DT_NODELABEL(led_head_0), gpios),
        .config = GPIO_OUTPUT_INACTIVE,
    },
    [LED_HEAD_1] = {
        .dt = GPIO_DT_SPEC_GET(DT_NODELABEL(led_head_1), gpios),
        .config = GPIO_OUTPUT_INACTIVE,
    },
    [LED_TAIL_0] = {
        .dt = GPIO_DT_SPEC_GET(DT_NODELABEL(led_tail_0), gpios),
        .config = GPIO_OUTPUT_INACTIVE,
    },
    [LED_TAIL_1] = {
        .dt = GPIO_DT_SPEC_GET(DT_NODELABEL(led_tail_1), gpios),
        .config = GPIO_OUTPUT_INACTIVE,
    },
    [LED_CHASIS] = {
        .dt = GPIO_DT_SPEC_GET(DT_NODELABEL(led_chasis), gpios),
        .config = GPIO_OUTPUT_INACTIVE,
    },
    [LED_FIRE] = {
        .dt = GPIO_DT_SPEC_GET(DT_NODELABEL(led_fire), gpios),
        .config = GPIO_OUTPUT_INACTIVE,
    },
};

void led_set(led_t id, bool en) {
  if (id >= LED_HEAD_0 && id < LED_PIN_MAX) {
    if ((id != LED_CHASIS) || ((prj_cfg->mode == 0) && (id == LED_CHASIS))) {
      gpio_pin_set_dt(&led_pins[id].dt, en);
    }
  } else {
    printf("Unknow id: %d\n", id);
  }
}

#include <zephyr/init.h>

static int led_init(void) {
    int ret = 0;

    LOG_INF("Initializing LED...");


    // Init LED
    for (size_t i = 0; i < ARRAY_SIZE(led_pins); i++) {
        const struct gpio_dt_spec *dt = &led_pins[i].dt;
        uint32_t *config = (uint32_t *)&led_pins[i].config;

        ret = gpio_pin_configure_dt(dt, *config);
        if (ret < 0) {
            LOG_ERR("Error %d: failed to configure %s pin %d", ret,
                    dt->port->name, dt->pin);
            return -EIO;
        }
    }

    return ret;
}

SYS_INIT(led_init, APPLICATION, SYS_INIT_NORMAL);