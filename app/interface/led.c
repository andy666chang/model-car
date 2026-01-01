
#include <system.h>
#include "module/log.h"
#include "interface.h"

#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#define TAG "LED"

enum {
    LED_HEAD_0 = 0,
    LED_HEAD_1,
    LED_TAIL_0,
    LED_TAIL_1,
    LED_CHASIS,
    LED_FIRE,

    LED_PIN_MAX,
};

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

void led_head_set(uint8_t id, bool en) {

    switch (id) {
    case 0:
        gpio_pin_set_dt(&led_pins[LED_HEAD_0].dt, en);
        break;
    
    case 1:
        gpio_pin_set_dt(&led_pins[LED_HEAD_1].dt, en);
        break;
    
    default:
        printf("Unknow id: %d\n", id);
        break;
    }
}

void led_tail_set(uint8_t id, bool en) {

    switch (id) {
    case 0:
        gpio_pin_set_dt(&led_pins[LED_TAIL_0].dt, en);
        break;
    
    case 1:
        gpio_pin_set_dt(&led_pins[LED_TAIL_1].dt, en);
        break;
    
    default:
        printf("Unknow id: %d\n", id);
        break;
    }
}

void led_chasis_set(bool en) {
    if (prj_cfg->mode == 0) {
        gpio_pin_set_dt(&led_pins[LED_CHASIS].dt, en);
    }
}

void led_fire_set(bool en) {
    gpio_pin_set_dt(&led_pins[LED_FIRE].dt, en);
}

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
