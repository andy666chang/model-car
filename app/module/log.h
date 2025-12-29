#ifndef _LOG_H_
#define _LOG_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    LOG_NONE,  /*!< No log output */
    LOG_ERROR, /*!< Critical errors, software module can not recover on its
                  own */
    LOG_WARN,  /*!< Error conditions from which recovery measures have been
                  taken */
    LOG_INFO,  /*!< Information messages which describe normal flow of events
                */
    LOG_DEBUG, /*!< Extra information which is not necessary for normal use
                  (values, pointers, sizes, etc). */
    LOG_VERBOSE, /*!< Bigger chunks of debugging information, or frequent
                   messages which can potentially flood the output. */
    LOG_LEVLE_MAX,
} log_level_t;


typedef uint32_t (*log_system_ms)(void);

/**
 * @brief Route the system uptime function into the log
 */
void log_route(log_system_ms system_ms);

/**
 * @brief Write message into the log
 *
 * This function is not intended to be used directly. Instead, use one of
 * LOGE, LOGW, LOGI, LOGD, LOGV macros.
 *
 * This function or these macros should not be used from an interrupt.
 */
void log_write(log_level_t level, const char *tag, const char *format,
                  ...);

/**
 * @brief Function which returns timestamp to be used in log output
 *
 * @return timestamp, in milliseconds
 */
uint32_t log_timestamp(void);

#define LOG_COLOR_BLACK "30"
#define LOG_COLOR_RED "31"
#define LOG_COLOR_GREEN "32"
#define LOG_COLOR_BROWN "33"
#define LOG_COLOR_BLUE "34"
#define LOG_COLOR_PURPLE "35"
#define LOG_COLOR_CYAN "36"
#define LOG_COLOR(COLOR) "\033[0;" COLOR "m"
#define LOG_BOLD(COLOR) "\033[1;" COLOR "m"
#define LOG_RESET_COLOR "\033[0m"
#define LOG_COLOR_E LOG_COLOR(LOG_COLOR_RED)
#define LOG_COLOR_W LOG_COLOR(LOG_COLOR_BROWN)
#define LOG_COLOR_I LOG_COLOR(LOG_COLOR_GREEN)
#define LOG_COLOR_D
#define LOG_COLOR_V

#define LOG_FORMAT(letter, format)                                             \
    LOG_COLOR_##letter #letter " (%010d) %s-L%d: " format LOG_RESET_COLOR "\r\n"

#define LOG_ERR(format, ...) LOGE(TAG, format, ##__VA_ARGS__)
#define LOG_WRN(format, ...) LOGW(TAG, format, ##__VA_ARGS__)
#define LOG_INF(format, ...) LOGI(TAG, format, ##__VA_ARGS__)
#define LOG_DBG(format, ...) LOGD(TAG, format, ##__VA_ARGS__)
#define LOG_VER(format, ...) LOGV(TAG, format, ##__VA_ARGS__)

#define LOGE(tag, format, ...)                                              \
    LOG_LEVEL_LOCAL(LOG_ERROR, tag, format, ##__VA_ARGS__)
#define LOGW(tag, format, ...)                                              \
    LOG_LEVEL_LOCAL(LOG_WARN, tag, format, ##__VA_ARGS__)
#define LOGI(tag, format, ...)                                              \
    LOG_LEVEL_LOCAL(LOG_INFO, tag, format, ##__VA_ARGS__)
#define LOGD(tag, format, ...)                                              \
    LOG_LEVEL_LOCAL(LOG_DEBUG, tag, format, ##__VA_ARGS__)
#define LOGV(tag, format, ...)                                              \
    LOG_LEVEL_LOCAL(LOG_VERBOSE, tag, format, ##__VA_ARGS__)

/** runtime macro to output logs at a specified level.
 *
 * @param tag tag of the log, which can be used to change the log level by
 * ``log_level_set`` at runtime.
 * @param level level of the output log.
 * @param format format of the output log. see ``printf``
 * @param ... variables to be replaced into the log. see ``printf``
 *
 * @see ``printf``
 */
#define LOG_LEVEL(level, tag, format, ...)                                  \
    do {                                                                       \
        if (level == LOG_ERROR) {                                           \
            log_write(LOG_ERROR, tag, LOG_FORMAT(E, format),             \
                         log_timestamp(), tag, __LINE__, ##__VA_ARGS__);    \
        } else if (level == LOG_WARN) {                                     \
            log_write(LOG_WARN, tag, LOG_FORMAT(W, format),              \
                         log_timestamp(), tag, __LINE__, ##__VA_ARGS__);    \
        } else if (level == LOG_DEBUG) {                                    \
            log_write(LOG_DEBUG, tag, LOG_FORMAT(D, format),             \
                         log_timestamp(), tag, __LINE__, ##__VA_ARGS__);    \
        } else if (level == LOG_VERBOSE) {                                  \
            log_write(LOG_VERBOSE, tag, LOG_FORMAT(V, format),           \
                         log_timestamp(), tag, __LINE__, ##__VA_ARGS__);    \
        } else {                                                               \
            log_write(LOG_INFO, tag, LOG_FORMAT(I, format),              \
                         log_timestamp(), tag, __LINE__, ##__VA_ARGS__);    \
        }                                                                      \
    } while (0)

/** runtime macro to output logs at a specified level. Also check the level with
 * ``LOG_LOCAL_LEVEL``.
 *
 * @see ``printf``, ``LOG_LEVEL``
 */
#define LOG_LEVEL_LOCAL(level, tag, format, ...)                            \
    do {                                                                       \
        LOG_LEVEL(level, tag, format, ##__VA_ARGS__);                       \
    } while (0)

void log_set_level(log_level_t level);
log_level_t log_get_level(void);
const char *log_get_log_level_name(log_level_t level);

#ifdef __cplusplus
}
#endif

#endif /* MCU_DRIVER_LOG_LOG_H_ */
