#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interface/interface.h"
#include "interface/led.h"
#include "interface/led_bar.h"
#include "module/log.h"

static prj_cfg_t simulator_default_cfg = {
    .version = 0,
    .dir = 1,
    .center = 1500,
    .margin = 10,
    .max = 2000,
    .mode = 0,
    .bar_idx = 0,
};

prj_cfg_t *prj_cfg = &simulator_default_cfg;

#define TAG "SIM"

static void simulator_send_shell_command(const char *cmd) {
  if (cmd == NULL) {
    return;
  }

  while (*cmd != '\0') {
    shell_data_push((uint8_t)*cmd);
    cmd++;
  }

  shell_data_push('\r');
}

static void simulator_press_button(uint8_t count) {
  static uint16_t btn = 2000;

  for (uint8_t i = 0; i < count; i++) {
    btn_data_push(btn);
    btn = (btn == 2000) ? 1000 : 2000;
  }
}

static void simulator_set_throttle(int16_t value) {
  if (value < 1000) {
    value = 1000;
  }
  if (value > 2000) {
    value = 2000;
  }

  thro_data_push((uint16_t)value);
}

static int shell_sim(int argc, char *argv[]) {
  int btn_count = 0;
  int thro_value = -1;
  int led_string = -1;
  int do_update = 0;
  int do_brake = 0;
  const char *cmd = NULL;

  for (int i = 0; i < argc; i++) {
    if (!strcmp(argv[i], "-b") && (i + 1) < argc) {
      btn_count = strtol(argv[i + 1], NULL, 10);
      continue;
    }

    if (!strcmp(argv[i], "-t") && (i + 1) < argc) {
      thro_value = strtol(argv[i + 1], NULL, 10);
      continue;
    }

    if (!strcmp(argv[i], "-s") && (i + 1) < argc) {
      led_string = strtol(argv[i + 1], NULL, 10);
      continue;
    }

    if (!strcmp(argv[i], "-c") && (i + 1) < argc) {
      cmd = argv[i + 1];
      continue;
    }

    if (!strcmp(argv[i], "-u")) {
      do_update = 1;
      continue;
    }

    if (!strcmp(argv[i], "-r")) {
      do_brake = 1;
      continue;
    }
  }

  if (btn_count > 0) {
    LOGI(TAG, "simulator button press count: %d", btn_count);
    simulator_press_button((uint8_t)btn_count);
  }

  if (thro_value >= 0) {
    LOGI(TAG, "simulator throttle value: %d", thro_value);
    simulator_set_throttle((int16_t)thro_value);
  }

  if (led_string >= 0) {
    LOGI(TAG, "simulator led string set: %d", led_string);
    led_string_set((bool)led_string);
  }

  if (cmd != NULL) {
    LOGI(TAG, "simulator shell command: %s", cmd);
    simulator_send_shell_command(cmd);
  }

  if (do_update) {
    if (prj_cfg != NULL) {
      LOGI(TAG, "simulator update throttle led");
      thro_led_update(prj_cfg->max);
    } else {
      LOGW(TAG, "prj_cfg not ready, skip throttle led update");
    }
  }

  if (do_brake) {
    LOGI(TAG, "simulator throttle brake");
    thro_led_brake();
  }

  return 0;
}

void load_config(void) {
  /* simulator uses default static config */
  prj_cfg = (prj_cfg_t *)&simulator_default_cfg;
}

void save_config(void) { /* no-op for simulator */ }

void dump_config(void) {
  LOGI(TAG,
       "simulator config: version=%d dir=%d center=%d margin=%d max=%d mode=%d "
       "bar_idx=%d",
       prj_cfg->version, prj_cfg->dir, prj_cfg->center, prj_cfg->margin,
       prj_cfg->max, prj_cfg->mode, prj_cfg->bar_idx);
}

uint32_t crc_calculate(uint8_t *data, uint32_t len) {
  uint32_t crc = 0;
  for (uint32_t i = 0; i < len; i++) {
    crc += data[i];
  }
  return crc;
}

void led_set(led_t id, bool en) {
  LOGI(TAG, "simulator stub led_set(id=%d, en=%d)", id, (int)en);
}

void led_string_set(bool en) {
  LOGI(TAG, "simulator stub led_string_set(%d)", (int)en);
}

void thro_led_update(int16_t thro) {
  LOGI(TAG, "simulator stub thro_led_update(%d)", (int)thro);
}

void thro_led_brake(void) { LOGI(TAG, "simulator stub thro_led_brake"); }

void thro_led_set(uint8_t r, uint8_t g, uint8_t b) {
  LOGI(TAG, "simulator stub thro_led_set(r=%d,g=%d,b=%d)", r, g, b);
}

uint8_t led_idx_max(void) { return 10; }

#include <module/shell.h>
#include <zephyr/kernel.h>

STRUCT_SECTION_ITERABLE(shell_t, sim) = {
    .name = "sim",
    .info = "sim -b <count> -t <1000..2000> -s <0|1> -c <cmd> -u -r",
    .func = shell_sim,
    .sub = NULL,
};
