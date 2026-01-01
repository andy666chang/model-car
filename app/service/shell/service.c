
#include <errno.h>

#include "system.h"
#include <module/shell.h>
#include <module/log.h>
#include "interface/interface.h"

#include <zephyr/drivers/uart.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/sys/ring_buffer.h>

#define TAG "SHELL-S"

#define SHELL_TIMEOUT 20

RING_BUF_DECLARE(sh_buf, 32);

void shell_data_push(uint8_t data) {
    ring_buf_put(&sh_buf, &data, sizeof(data));
    LOG_DBG("uart: %c", data);
}

/**
 * @brief 
 * 
 */
void shell_service_process(void) {
    static uint32_t sh_time = 0;

    if (WAIT_TIMEOUT(sh_time, SHELL_TIMEOUT)) {
        if (!ring_buf_is_empty(&sh_buf)) {
            uint16_t len = 0;
            uint8_t buf[10] = {0};
            uint8_t *p_buf = buf;
            while (!ring_buf_is_empty(&sh_buf) && len < sizeof(buf)) {
                ring_buf_get(&sh_buf, p_buf, sizeof(uint8_t));
                p_buf++;
                len++;
            }

            shell_process(buf, len);
        }
        sh_time = GET_SYS_TIME();
    }
}
