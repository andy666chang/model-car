/*
 * @Author: andy.chang 
 * @Date: 2024-12-31 15:01:03 
 * @Last Modified by: andy.chang
 * @Last Modified time: 2024-12-31 15:08:47
 */

#include "shell_sys.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <zephyr/sys/reboot.h>

#include "module/log.h"

#define TAG "SHELL-SYS"

static int shell_reboot(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    sys_reboot(SYS_REBOOT_COLD);
    return 0;
}

struct shell_t _sys_list[] = {
    { "reboot", "reboot system", shell_reboot, NULL},
    SHELL_END,
};
