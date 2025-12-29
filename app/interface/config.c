
#include "module/log.h"
#include "interface.h"

#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>

#define TAG "CONFIG"

#define MAGIC 0xA1234567

typedef struct cfg_pack_t {
    union pack_t {
        prj_cfg_t cfg;
        uint8_t dummy[252];
    };

    union pack_t pack;
    uint32_t check; // CRC32
} cfg_pack_t;

static const prj_cfg_t default_cfg = {
    .version = 0,
    .dir = 1,
    .center = 1500,
    .margin = 10,
    .max = 2000,
    
    .mode = 0,
    .bar_idx = 0,
};

static cfg_pack_t cfg_pack = {.check = 0x55};
prj_cfg_t *prj_cfg = NULL;

#define DATA0_OFFSET DT_REG_ADDR(DT_NODELABEL(data0)) - DT_REG_ADDR(DT_NODELABEL(flash0))
#define DATA0_SIZE   DT_REG_SIZE(DT_NODELABEL(data0))
const struct device *flash_dev = DEVICE_DT_GET(DT_NODELABEL(flash));

void load_config(void) {
    int ret = flash_read(flash_dev, DATA0_OFFSET, (uint8_t *)&cfg_pack, sizeof(cfg_pack_t));
    if (ret)
        LOG_ERR("Flash read error: %d", ret);
}

void save_config(void) {
    int ret = 0;

    // TODO: Caculate CRC32
    cfg_pack.check = MAGIC;

    // Erase and write to flash
    ret = flash_erase(flash_dev, DATA0_OFFSET, DATA0_SIZE);
    if (ret)
        LOG_ERR("Flash erase error: %d", ret);

    ret = flash_write(flash_dev, DATA0_OFFSET, (uint8_t *)&cfg_pack, sizeof(cfg_pack_t));
    if (ret)
        LOG_ERR("Flash write error: %d", ret);
}

void dump_config(void) {
    LOG_INF("prj_cfg:");
    LOG_INF("  version = %d", prj_cfg->version);
    LOG_INF("  dir = %d", prj_cfg->dir);
    LOG_INF("  center = %d", prj_cfg->center);
    LOG_INF("  margin = %d", prj_cfg->margin);
    LOG_INF("  max = %d", prj_cfg->max);
    LOG_INF("  mode = %d", prj_cfg->mode);
    LOG_INF("  bar_idx = %d", prj_cfg->bar_idx);
}

static int config_init(void) {
    // LOG_INF("size of prj_cfg: %d", sizeof(prj_cfg_t)); // 10
    // LOG_INF("size of cfg_pack: %d", sizeof(cfg_pack_t)); // 256

    // Read config pack from flash
    load_config();
    LOG_INF("cfg_pack.check: 0x%08X", cfg_pack.check);

    // TODO: verify data valid with CRC32
    if (cfg_pack.check != MAGIC) {
        // Replace by default config
        LOG_INF("Invalid data in flash, reset to default");
        memset(&cfg_pack, 0, sizeof(cfg_pack_t));
        memcpy(&cfg_pack.pack.cfg, &default_cfg, sizeof(prj_cfg_t));
        cfg_pack.check = MAGIC;

        // Erase and write to flash
        save_config();
    }
    
    // Load to global config
    prj_cfg = &cfg_pack.pack.cfg;

    // Dump config
    dump_config();

    return 0;
}

SYS_INIT(config_init, APPLICATION, 0);
