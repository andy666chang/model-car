/*
 * @Author: andy.chang 
 * @Date: 2024-12-31 15:01:03 
 * @Last Modified by: andy.chang
 * @Last Modified time: 2025-12-26 16:38:29
 */

#include "shell_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "interface/interface.h"

#include "module/log.h"

#define TAG "SHELL-CFG"

static int shell_dump(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    // Dump config
    dump_config();
    return 0;
}

static int shell_load(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    load_config();
    return 0;
}

static int shell_save(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    save_config();
    return 0;
}

static int shell_dir(int argc, char *argv[]) {
    (void) argc;

    int8_t dir = strtol(argv[0], NULL, 10);
    prj_cfg->dir = dir;
    LOGI(TAG, "dir = %d", prj_cfg->dir);
    return 0;
}

static int shell_center(int argc, char *argv[]) {
    (void) argc;

    uint16_t num = strtol(argv[0], NULL, 10);
    prj_cfg->center = num;
    LOGI(TAG, "center = %d", prj_cfg->center);
    return 0;
}

static int shell_margin(int argc, char *argv[]) {
    (void) argc;

    uint16_t num = strtol(argv[0], NULL, 10);
    prj_cfg->margin = num;
    LOGI(TAG, "margin = %d", prj_cfg->margin);
    return 0;
}

static int shell_max(int argc, char *argv[]) {
    (void) argc;

    uint16_t num = strtol(argv[0], NULL, 10);
    prj_cfg->max = num;
    LOGI(TAG, "max = %d", prj_cfg->max);
    return 0;
}

static int shell_mode(int argc, char *argv[]) {
    (void) argc;

    bool mode = strtol(argv[0], NULL, 10);
    prj_cfg->mode = mode;
    LOGI(TAG, "mode = %d", prj_cfg->mode);
    return 0;
}

static int shell_bar_idx(int argc, char *argv[]) {
    (void) argc;

    uint8_t bar_idx = strtol(argv[0], NULL, 10);
    prj_cfg->bar_idx = bar_idx;
    LOGI(TAG, "bar_idx = %d", prj_cfg->bar_idx);
    return 0;
}

struct shell_t _cfg_list[] = {
    { "dump", "dump config", shell_dump, NULL},
    { "load", "load config", shell_load, NULL},
    { "save", "save config", shell_save, NULL},
    { "dir" , "dir -1/1", shell_dir, NULL},
    { "center", "center <1500>", shell_center, NULL},
    { "margin", "margin <10>", shell_margin, NULL},
    { "max", "max <2000>", shell_max, NULL},
    { "mode", "mode <0/1>", shell_mode, NULL},
    { "bar_idx", "bar_idx <0-3>", shell_bar_idx, NULL},
    SHELL_END,
};
