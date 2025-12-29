
#include "module/log.h"
#include "interface.h"

#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/drivers/gpio.h>

#define TAG "RC"

static void sg_thro_cb(struct gpio_callback *cb);
static void sg_bt_cb(struct gpio_callback *cb);


static const struct device *cnt_dev = DEVICE_DT_GET(DT_NODELABEL(rc_cnt));

static struct rc_pin_t {
    struct gpio_dt_spec dt;
    struct gpio_callback cb_data;
} rc_pins[] = {
    {
        .dt = GPIO_DT_SPEC_GET(DT_NODELABEL(sg_thro), gpios),
    },
    {
        .dt = GPIO_DT_SPEC_GET(DT_NODELABEL(sg_bt), gpios),
    },
};

static uint32_t max_ticks = 0;
static uint16_t time[2] = {0}; // 1000 ~ 2000
static uint32_t ticks;
static void rc_pins_callback(const struct device *dev,
                              struct gpio_callback *cb, uint32_t pins) {
    uint16_t *ptime;
    void (*pdata_push)(uint16_t);
    counter_get_value(cnt_dev, &ticks);

    if (cb == &rc_pins[0].cb_data) {
        ptime = &time[0];
        pdata_push = thro_data_push;
    } else if (cb == &rc_pins[1].cb_data) {
        ptime = &time[1];
        pdata_push = btn_data_push;
    } else {
        return;
    }

    struct rc_pin_t *rc_pin = CONTAINER_OF(cb, struct rc_pin_t, cb_data);
    int level = gpio_pin_get_dt(&rc_pin->dt);

    if (level) {
        *ptime = ticks;
    } else {
        int16_t diff = ticks - *ptime;

        if (diff < 0)
            diff += max_ticks;
        
        // Sent to service
        pdata_push(diff);
    }
}

static int rc_init(void) {
    int ret = 0;

    LOG_INF("Initializing RC interface...");

    // Init timer
    counter_start(cnt_dev);
    max_ticks = counter_get_max_top_value(cnt_dev);
    LOG_INF("Timer started");
    LOG_INF("  Frequency: %u Hz", counter_get_frequency(cnt_dev));
    LOG_INF("  MAX: %u ticks", max_ticks);

    // Init RC input
    for (size_t i = 0; i < ARRAY_SIZE(rc_pins); i++) {
        const struct gpio_dt_spec *dt = &rc_pins[i].dt;
        struct gpio_callback *cb_data = &rc_pins[i].cb_data;

        ret = gpio_pin_configure_dt(dt, GPIO_INPUT | GPIO_PULL_UP);
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
