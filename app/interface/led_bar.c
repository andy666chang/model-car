
#include <stdio.h>
#include <math.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <stm32_ll_gpio.h>
#include <zephyr/logging/log.h>
#include "interface.h"

LOG_MODULE_REGISTER(led_bar, LOG_LEVEL_INF);

enum {
    LED_BAR_SIG = 0,
    LED_FW_SW,

    LED_PIN_MAX,
};

enum {
    BAR_THRO = 0,
    BAR_WHITE,
    BAR_RED,
    BAR_BRAKE = BAR_RED,
    BAR_ORANGE,
    BAR_YELLOW,
    BAR_GREEN,
    BAR_CYAN,
    BAR_BLUE,
    BAR_PURPLE,
    BAR_PINK,

    BAR_MAX,
};

static const struct led_pin_t {
    struct gpio_dt_spec dt;
    uint32_t config;
} led_pins[] = {
    [LED_BAR_SIG] = {
        .dt = GPIO_DT_SPEC_GET(DT_NODELABEL(led_chasis), gpios),
        .config = GPIO_OUTPUT_INACTIVE,
    },
    [LED_FW_SW] = {
        .dt = GPIO_DT_SPEC_GET(DT_NODELABEL(fw_sw), gpios),
        .config = GPIO_OUTPUT_INACTIVE,
    },
};

const struct gpio_dt_spec *led_spec = &led_pins[LED_BAR_SIG].dt;
static GPIO_TypeDef *gpio_port = (GPIO_TypeDef *)DT_REG_ADDR(DT_GPIO_CTLR(DT_NODELABEL(led_chasis), gpios));
static uint32_t ll_pin;

union led_pack_t {
    struct led_data_t {
        // Little endian
        uint8_t b;
        uint8_t r;
        uint8_t g;
        uint8_t rev;
    } color;

    uint32_t data;
};

static const union led_pack_t led_ref[BAR_MAX][12] =
    {
        [BAR_THRO] =
            {
                {.color = {.r =  0, .g = 60, .b = 0}},
                {.color = {.r = 10, .g = 50, .b = 0}},
                {.color = {.r = 20, .g = 40, .b = 0}},
                {.color = {.r = 30, .g = 30, .b = 0}},
                {.color = {.r = 40, .g = 25, .b = 0}},
                {.color = {.r = 50, .g = 20, .b = 0}},
                {.color = {.r = 60, .g = 15, .b = 0}},
                {.color = {.r = 70, .g = 15, .b = 0}},
                {.color = {.r = 80, .g = 10, .b = 0}},
                {.color = {.r = 90, .g =  5, .b = 0}},
                {.color = {.r = 100, .g = 0, .b = 0}},
                {.color = {.r = 100, .g = 0, .b = 0}},
            },
        [BAR_WHITE] =
            {
                {.color = {.r = 20, .g = 15, .b = 20}},
                {.color = {.r = 20, .g = 15, .b = 20}},
                {.color = {.r = 20, .g = 15, .b = 20}},
                {.color = {.r = 20, .g = 15, .b = 20}},
                {.color = {.r = 20, .g = 15, .b = 20}},
                {.color = {.r = 20, .g = 15, .b = 20}},
                {.color = {.r = 20, .g = 15, .b = 20}},
                {.color = {.r = 20, .g = 15, .b = 20}},
                {.color = {.r = 20, .g = 15, .b = 20}},
                {.color = {.r = 20, .g = 15, .b = 20}},
                {.color = {.r = 20, .g = 15, .b = 20}},
                {.color = {.r = 20, .g = 15, .b = 20}},
            },
        [BAR_RED] =
            {
                {.color = {.r = 100, .g = 0, .b = 0}},
                {.color = {.r = 100, .g = 0, .b = 0}},
                {.color = {.r = 100, .g = 0, .b = 0}},
                {.color = {.r = 100, .g = 0, .b = 0}},
                {.color = {.r = 100, .g = 0, .b = 0}},
                {.color = {.r = 100, .g = 0, .b = 0}},
                {.color = {.r = 100, .g = 0, .b = 0}},
                {.color = {.r = 100, .g = 0, .b = 0}},
                {.color = {.r = 100, .g = 0, .b = 0}},
                {.color = {.r = 100, .g = 0, .b = 0}},
                {.color = {.r = 100, .g = 0, .b = 0}},
                {.color = {.r = 100, .g = 0, .b = 0}},
            },
        [BAR_ORANGE] =
            {
                {.color = {.r = 60, .g = 20, .b = 0}},
                {.color = {.r = 60, .g = 20, .b = 0}},
                {.color = {.r = 60, .g = 20, .b = 0}},
                {.color = {.r = 60, .g = 20, .b = 0}},
                {.color = {.r = 60, .g = 20, .b = 0}},
                {.color = {.r = 60, .g = 20, .b = 0}},
                {.color = {.r = 60, .g = 20, .b = 0}},
                {.color = {.r = 60, .g = 20, .b = 0}},
                {.color = {.r = 60, .g = 20, .b = 0}},
                {.color = {.r = 60, .g = 20, .b = 0}},
                {.color = {.r = 60, .g = 20, .b = 0}},
                {.color = {.r = 60, .g = 20, .b = 0}},
            },
        [BAR_YELLOW] =
            {
                {.color = {.r = 60, .g = 40, .b = 0}},
                {.color = {.r = 60, .g = 40, .b = 0}},
                {.color = {.r = 60, .g = 40, .b = 0}},
                {.color = {.r = 60, .g = 40, .b = 0}},
                {.color = {.r = 60, .g = 40, .b = 0}},
                {.color = {.r = 60, .g = 40, .b = 0}},
                {.color = {.r = 60, .g = 40, .b = 0}},
                {.color = {.r = 60, .g = 40, .b = 0}},
                {.color = {.r = 60, .g = 40, .b = 0}},
                {.color = {.r = 60, .g = 40, .b = 0}},
                {.color = {.r = 60, .g = 40, .b = 0}},
                {.color = {.r = 60, .g = 40, .b = 0}},
            },
        [BAR_GREEN] =
            {
                {.color = {.r = 0, .g = 60, .b = 0}},
                {.color = {.r = 0, .g = 60, .b = 0}},
                {.color = {.r = 0, .g = 60, .b = 0}},
                {.color = {.r = 0, .g = 60, .b = 0}},
                {.color = {.r = 0, .g = 60, .b = 0}},
                {.color = {.r = 0, .g = 60, .b = 0}},
                {.color = {.r = 0, .g = 60, .b = 0}},
                {.color = {.r = 0, .g = 60, .b = 0}},
                {.color = {.r = 0, .g = 60, .b = 0}},
                {.color = {.r = 0, .g = 60, .b = 0}},
                {.color = {.r = 0, .g = 60, .b = 0}},
                {.color = {.r = 0, .g = 60, .b = 0}},
            },
        [BAR_CYAN] =
            {
                {.color = {.r = 0, .g = 25, .b =  25}},
                {.color = {.r = 0, .g = 25, .b =  25}},
                {.color = {.r = 0, .g = 25, .b =  25}},
                {.color = {.r = 0, .g = 25, .b =  25}},
                {.color = {.r = 0, .g = 25, .b =  25}},
                {.color = {.r = 0, .g = 25, .b =  25}},
                {.color = {.r = 0, .g = 25, .b =  25}},
                {.color = {.r = 0, .g = 25, .b =  25}},
                {.color = {.r = 0, .g = 25, .b =  25}},
                {.color = {.r = 0, .g = 25, .b =  25}},
                {.color = {.r = 0, .g = 25, .b =  25}},
                {.color = {.r = 0, .g = 25, .b =  25}},
            },
        [BAR_BLUE] =
            {
                {.color = {.r = 0, .g = 0, .b = 100}},
                {.color = {.r = 0, .g = 0, .b = 100}},
                {.color = {.r = 0, .g = 0, .b = 100}},
                {.color = {.r = 0, .g = 0, .b = 100}},
                {.color = {.r = 0, .g = 0, .b = 100}},
                {.color = {.r = 0, .g = 0, .b = 100}},
                {.color = {.r = 0, .g = 0, .b = 100}},
                {.color = {.r = 0, .g = 0, .b = 100}},
                {.color = {.r = 0, .g = 0, .b = 100}},
                {.color = {.r = 0, .g = 0, .b = 100}},
                {.color = {.r = 0, .g = 0, .b = 100}},
                {.color = {.r = 0, .g = 0, .b = 100}},
            },
        [BAR_PURPLE] =
            {
                {.color = {.r = 10, .g = 0, .b = 25}},
                {.color = {.r = 10, .g = 0, .b = 25}},
                {.color = {.r = 10, .g = 0, .b = 25}},
                {.color = {.r = 10, .g = 0, .b = 25}},
                {.color = {.r = 10, .g = 0, .b = 25}},
                {.color = {.r = 10, .g = 0, .b = 25}},
                {.color = {.r = 10, .g = 0, .b = 25}},
                {.color = {.r = 10, .g = 0, .b = 25}},
                {.color = {.r = 10, .g = 0, .b = 25}},
                {.color = {.r = 10, .g = 0, .b = 25}},
                {.color = {.r = 10, .g = 0, .b = 25}},
                {.color = {.r = 10, .g = 0, .b = 25}},
            },
        [BAR_PINK] =
            {
                {.color = {.r = 30, .g = 0, .b = 15}},
                {.color = {.r = 30, .g = 0, .b = 15}},
                {.color = {.r = 30, .g = 0, .b = 15}},
                {.color = {.r = 30, .g = 0, .b = 15}},
                {.color = {.r = 30, .g = 0, .b = 15}},
                {.color = {.r = 30, .g = 0, .b = 15}},
                {.color = {.r = 30, .g = 0, .b = 15}},
                {.color = {.r = 30, .g = 0, .b = 15}},
                {.color = {.r = 30, .g = 0, .b = 15}},
                {.color = {.r = 30, .g = 0, .b = 15}},
                {.color = {.r = 30, .g = 0, .b = 15}},
                {.color = {.r = 30, .g = 0, .b = 15}},
            },
};


void led_string_set(bool en) {
    gpio_pin_set_dt(&led_pins[LED_FW_SW].dt, en);
}

static void led_reset(void) {
    // Trst: Low level > 80us
    LL_GPIO_ResetOutputPin(gpio_port, ll_pin);
    for (size_t i = 0; i < 800; i++) {
        arch_nop();
    }
}

static inline void led_set_low(void) {
    // T0H: 0.4 +- 0.15 us
    LL_GPIO_SetOutputPin(gpio_port, ll_pin);
    arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop();
    arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop();

    // T0L: 0.85 +- 0.15 us
    LL_GPIO_ResetOutputPin(gpio_port, ll_pin);
    arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop();
    arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop();
    arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop();
    arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop();
}

/**
 * @brief 
 * 
 */
static inline void led_set_high(void) {
    // T1H: 0.8 +- 0.15 us
    LL_GPIO_SetOutputPin(gpio_port, ll_pin);
    arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop();
    arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop();
    arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop();
    arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop();
    arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop();

    // T1L: 0.45 +- 0.15 us
    LL_GPIO_ResetOutputPin(gpio_port, ll_pin);
    arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop();
    arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); arch_nop(); 
}

static inline void led_data_set(uint32_t data) {
    LOG_DBG("led_data: 0x%06X", data);

    for (int8_t i = 0; i < 24; i++) {
        if (data & (0x800000)) {
            led_set_high();
        } else {
            led_set_low();
        }
        data <<= 1;
    }
}

static void led_color_set(uint8_t r, uint8_t g, uint8_t b) {
    led_data_set(((uint32_t)g << 16) | ((uint32_t)r << 8) | ((uint32_t)b));
}

void thro_led_update(int16_t thro) {
    uint8_t idx = prj_cfg->bar_idx;
    uint8_t step = floorf((prj_cfg->max - prj_cfg->center) / 12.0);
    uint8_t on, off;

    if (idx >= ARRAY_SIZE(led_ref)) {
        LOG_ERR("Invalid bar idx: %d", idx);
        idx = 0;
    }
    thro = MAX(0, thro);

    on = MIN((thro / step), 12);
    on = MAX(on, 1);
    off = 12 - on;

    LOG_DBG("thro: %d, on: %d, off: %d", thro, on, off);

    unsigned int key = irq_lock();
    for (size_t i = 0; i < on; i++) {
        led_data_set(led_ref[idx][i].data);
    }

    for (size_t i = 0; i < off; i++) {
        led_data_set(0x000000);
    }
    irq_unlock(key);

    led_reset();
}

void thro_led_brake(void) {
    unsigned int key = irq_lock();
    for (size_t i = 0; i < 12; i++) {
        led_data_set(led_ref[BAR_BRAKE][i].data);
    }
    irq_unlock(key);

    led_reset();
}

void thro_led_set(uint8_t r, uint8_t g, uint8_t b) {
    unsigned int key = irq_lock();
    for (size_t i = 0; i < 12; i++) {
        led_color_set(r, g, b);
    }
    irq_unlock(key);

    led_reset();
}

uint8_t led_idx_max(void) {
    return ARRAY_SIZE(led_ref);
}

static int led_bar_init(void) {
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

    ll_pin = (1 << led_spec->pin);
    if (prj_cfg->mode == 1) {
        led_string_set(1);
        LL_GPIO_ResetOutputPin(gpio_port, ll_pin);
    }

    return ret;
}

SYS_INIT(led_bar_init, APPLICATION, 0);
