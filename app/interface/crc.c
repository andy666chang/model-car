
#include "interface.h"
#include <zephyr/sys/crc.h>


// stm32 cube default: 0x8414D179
// Zephyr default: 0x70A1FDDA
uint32_t crc_calculate(uint8_t *data, uint32_t len) {
    return crc32_ieee(data, len);
}
