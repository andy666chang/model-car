
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

#ifndef CONFIG_MULTITHREADING
RING_BUF_DECLARE(sh_buf, 32);
#else
struct sh_msg_t {
    void *fifo_reserved; 
    uint8_t data;
};

K_MEM_SLAB_DEFINE_STATIC(sh_msg_slab, sizeof(struct sh_msg_t), 16, 2);

K_FIFO_DEFINE(sh_fifo);
#endif

void shell_data_push(uint8_t data) {
#ifndef CONFIG_MULTITHREADING
    ring_buf_put(&sh_buf, &data, sizeof(data));
    LOG_DBG("uart: %c", data);
#else
    struct sh_msg_t *pmsg;

    if (k_mem_slab_alloc(&sh_msg_slab, (void **)&pmsg, K_NO_WAIT) == 0) {
        pmsg->data = data;
        k_fifo_put(&sh_fifo, pmsg);
    } else {
        LOG_ERR("k_mem_slab_alloc fail!");
    }
#endif
}

#ifndef CONFIG_MULTITHREADING
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
#endif

#if CONFIG_MULTITHREADING

static void sh_service(void) {
    while (1) {
        struct sh_msg_t *pmsg;
        pmsg = k_fifo_get(&sh_fifo, K_FOREVER);

        if (pmsg != NULL) {
            uint8_t data = pmsg->data;
            k_mem_slab_free(&sh_msg_slab, (void *)pmsg);

            shell_process(&data, 1);
        }
    }
}

K_THREAD_DEFINE(sh_ser_id, APP_STACK_SIZE, sh_service, NULL, NULL, NULL,
        APP_PRIO_LOW, 0, 0);

#endif