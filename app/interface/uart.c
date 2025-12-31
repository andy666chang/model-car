
#include "module/log.h"
#include "interface.h"

#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define TAG "UART"

const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(usart1));

static void uart_cb(const struct device *dev, void *user_data) {
    uint8_t recv_char;

    // update irq status
    uart_irq_update(dev);

    // check (RX Ready)
    if (uart_irq_rx_ready(dev)) {
        // Read FIFO
        while (uart_fifo_read(dev, &recv_char, 1) > 0) {
            // Send data to shell
            shell_data_push(recv_char);
        }
    }
}

static int uart_init(void) {
    int ret = 0;

    LOG_INF("Initializing UART interface...");

    if (!device_is_ready(uart_dev)) {
        return -1;
    }

    // Config callback
    uart_irq_callback_user_data_set(uart_dev, uart_cb, NULL);

    // Enable irq
    uart_irq_rx_enable(uart_dev);


    return ret;
}

SYS_INIT(uart_init, APPLICATION, 0);
