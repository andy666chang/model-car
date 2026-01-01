
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <module/shell.h>
#include <module/log.h>

#include <zephyr/sys/reboot.h>

#define TAG "SHELL-SYS"

static int shell_reboot(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    sys_reboot(SYS_REBOOT_COLD);
    return 0;
}

static const struct shell_t sys_list[] = {
    { "reboot", "reboot system", shell_reboot, NULL},
    SHELL_END,
};

#include <zephyr/kernel.h>

STRUCT_SECTION_ITERABLE(shell_t, sys) = {
    .name = "sys",
    .info = "sys func",
    .func = NULL,
    .sub = sys_list,
};
