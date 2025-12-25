
#include <stdio.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include "interface/interface.h"

LOG_MODULE_REGISTER(rc, LOG_LEVEL_INF);

static void sg_thro_cb(struct gpio_callback *cb);
static void sg_bt_cb(struct gpio_callback *cb);


static const struct device *cnt_dev = DEVICE_DT_GET(DT_NODELABEL(rc_cnt));

typedef void (*rc_pin_cb_t)(struct gpio_callback *cb);
static struct rc_pin_t {
    struct gpio_dt_spec dt;
    struct gpio_callback cb_data;
    rc_pin_cb_t func;
} rc_pins[] = {
    {
        .dt = GPIO_DT_SPEC_GET(DT_NODELABEL(sg_thro), gpios),
        .func = sg_thro_cb,
    },
    {
        .dt = GPIO_DT_SPEC_GET(DT_NODELABEL(sg_bt), gpios),
        .func = sg_bt_cb,
    },
};

static uint32_t max_ticks = 0;
static uint16_t time[2] = {0}; // 1000 ~ 2000


static void sg_thro_cb(struct gpio_callback *cb) {
    struct rc_pin_t *rc_pin = CONTAINER_OF(cb, struct rc_pin_t, cb_data);
    int level = gpio_pin_get_dt(&rc_pin->dt);
    uint32_t ticks;

    if (level) {
        counter_get_value(cnt_dev, &ticks);
        time[0] = ticks;
    } else {
        counter_get_value(cnt_dev, &ticks);
        uint32_t diff = (ticks >= time[0]) ? (ticks - time[0]) : (max_ticks - time[0] + ticks + 1);
        
        // TODO: sent to service
        // printf("%s -> %4u\n", __func__, diff);
    }
}

static void sg_bt_cb(struct gpio_callback *cb) {
    struct rc_pin_t *rc_pin = CONTAINER_OF(cb, struct rc_pin_t, cb_data);
    int level = gpio_pin_get_dt(&rc_pin->dt);
    uint32_t ticks;

    if (level) {
        counter_get_value(cnt_dev, &ticks);
        time[1] = ticks;
    } else {
        counter_get_value(cnt_dev, &ticks);
        uint16_t diff = (ticks >= time[1]) ? (ticks - time[1]) : (max_ticks - time[1] + ticks + 1);
        
        // Sent to service
        // printf("%s -> %4u\n", __func__, diff);
        btn_data_push(diff);
    }
}

static void rc_pins_callback(const struct device *dev,
                              struct gpio_callback *cb, uint32_t pins) {
    struct rc_pin_t *rc_pin = CONTAINER_OF(cb, struct rc_pin_t, cb_data);
    if (rc_pin->func) {
        rc_pin->func(cb);
    }
}

static int rc_init(void) {
    int ret = 0;

    LOG_INF("Initializing RC interface...");

    // Init timer
    counter_start(cnt_dev);
    max_ticks = counter_get_max_top_value(cnt_dev);
    printf("Timer started\n");
    printf("  Frequency: %u Hz\n", counter_get_frequency(cnt_dev));
    printf("  MAX: %u ticks\n", max_ticks);

    // Init RC input
    for (size_t i = 0; i < ARRAY_SIZE(rc_pins); i++) {
        const struct gpio_dt_spec *dt = &rc_pins[i].dt;
        struct gpio_callback *cb_data = &rc_pins[i].cb_data;

        ret = gpio_pin_configure_dt(dt, GPIO_INPUT);
        if (ret < 0) {
            LOG_ERR("Error %d: failed to configure %s pin %d", ret,
                    dt->port->name, dt->pin);
            return -EIO;
        }

        ret = gpio_pin_interrupt_configure_dt(dt, GPIO_INT_EDGE_BOTH);
        if (ret != 0) {
            LOG_ERR("Error %d: failed to configure interrupt on %s pin %d",
                    ret, dt->port->name, dt->pin);
            return -EIO;
        }

        gpio_init_callback(cb_data, rc_pins_callback, BIT(dt->pin));
        gpio_add_callback(dt->port, cb_data);
        LOG_INF("Set up INT at %s pin %d, level: %d", dt->port->name,
                dt->pin, gpio_pin_get_dt(dt));
    }


    return ret;
}

SYS_INIT(rc_init, APPLICATION, 0);
