/*
 * @Author: andy.chang 
 * @Date: 2024-08-03 01:02:21 
 * @Last Modified by: andy.chang
 * @Last Modified time: 2024-12-05 21:22:48
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <ctype.h>

#include <errno.h>

#include "shell.h"
#include "log.h"

#define TAG "SHELL"

#define SHELL_VT100_ASCII_ESC		(0x1b)
#define SHELL_VT100_ASCII_DEL		(0x7F)
#define SHELL_VT100_ASCII_BSPACE	(0x08)
#define SHELL_VT100_ASCII_CTRL_A	(0x01)
#define SHELL_VT100_ASCII_CTRL_B	(0x02)
#define SHELL_VT100_ASCII_CTRL_C	(0x03)
#define SHELL_VT100_ASCII_CTRL_D	(0x04)
#define SHELL_VT100_ASCII_CTRL_E	(0x05)
#define SHELL_VT100_ASCII_CTRL_F	(0x06)
#define SHELL_VT100_ASCII_CTRL_K	(0x0B)
#define SHELL_VT100_ASCII_CTRL_L	(0x0C)
#define SHELL_VT100_ASCII_CTRL_N	(0x0E)
#define SHELL_VT100_ASCII_CTRL_P	(0x10)
#define SHELL_VT100_ASCII_CTRL_U	(0x15)
#define SHELL_VT100_ASCII_CTRL_W	(0x17)
#define SHELL_VT100_ASCII_ALT_B		(0x62)
#define SHELL_VT100_ASCII_ALT_F		(0x66)
#define SHELL_VT100_ASCII_ALT_R		(0x72)

#define SHELL_ASCII_MAX_CHAR (127u)

enum
{
    SHELL_RECEIVE_DEFAULT,
    SHELL_RECEIVE_ESC,
    SHELL_RECEIVE_ESC_SEQ,
    SHELL_RECEIVE_TILDE_EXP,
};

#define SHELL_BUF_LEN 128
#define SHELL_HIS_LEN 5

int parse_shell(uint8_t *shell, uint32_t len);

static char shell_his[SHELL_HIS_LEN][SHELL_BUF_LEN] = {0};
static uint16_t h_idx = 0;

static char shell_buf[SHELL_BUF_LEN] = {0};
static uint16_t idx = 0;
static uint8_t state = SHELL_RECEIVE_DEFAULT;

static void shell_history_record(char *pdata, uint16_t len)
{
    memmove(shell_his[1], shell_his[0], SHELL_BUF_LEN * (SHELL_HIS_LEN - 1));
    memset(shell_his[0], 0, sizeof(shell_his[0]));
    memcpy(shell_his[0], pdata, len);
    h_idx = 0;
}

/**
 * @brief 
 * 
 * @param data 
 * @return int 
 */
static int ascii_filter(const char data)
{
    return (uint8_t) data > SHELL_ASCII_MAX_CHAR ? -EINVAL : 0;
}

/**
 * @brief 
 * 
 * @param sh 
 * @param data 
 */
static void ctrl_metakeys_handle(char data)
{
    switch (data)
    {
    case SHELL_VT100_ASCII_CTRL_A: /* CTRL + A */
    case SHELL_VT100_ASCII_CTRL_B: /* CTRL + B */
        break;
    case SHELL_VT100_ASCII_CTRL_C: /* CTRL + C */
        {
            printf("\33[2K\r");
            memmove(&shell_buf[idx], &shell_buf[idx+1], strlen(&shell_buf[idx+1])+1);
            printf("%s%s\r\n", "mcu-cli > ", shell_buf);

            memset(shell_buf, 0, sizeof(shell_buf));
            idx = 0;
        }
        break;

    case SHELL_VT100_ASCII_CTRL_D: /* CTRL + D */
    case SHELL_VT100_ASCII_CTRL_E: /* CTRL + E */
    case SHELL_VT100_ASCII_CTRL_F: /* CTRL + F */
    case SHELL_VT100_ASCII_CTRL_K: /* CTRL + K */
    case SHELL_VT100_ASCII_CTRL_L: /* CTRL + L */
    case SHELL_VT100_ASCII_CTRL_N: /* CTRL + N */
    case SHELL_VT100_ASCII_CTRL_P: /* CTRL + P */
    case SHELL_VT100_ASCII_CTRL_U: /* CTRL + U */
    case SHELL_VT100_ASCII_CTRL_W: /* CTRL + W */
    default:
        break;
    }
}

/**
 * @brief 
 * 
 * @param data 
 * @param len 
 */
void shell_process(uint8_t *data, uint16_t len)
{
    if (data == NULL)
        return;
    
    for (size_t i = 0; i < len; i++)
    {
        // filter ascii
        if (ascii_filter(data[i]) != 0) {
			continue;
		}

        switch (state)
        {
        case SHELL_RECEIVE_DEFAULT:
            if (data[i] == '\r')
            {
                printf("\33[2K\r");
                memmove(&shell_buf[idx], &shell_buf[idx+1], strlen(&shell_buf[idx+1])+1);
                printf("%s%s\r\n", "mcu-cli > ", shell_buf);

                // Save history
                if (strlen(shell_buf))
                {
                    shell_history_record(shell_buf, sizeof(shell_buf));
                }

                // Execute cmd
                LOGI(TAG, "shell: %s", shell_buf);
                parse_shell(shell_buf, sizeof(shell_buf));

                memset(shell_buf, 0, sizeof(shell_buf));
                idx = 0;
                continue;
            }

            switch (data[i])
            {
            case SHELL_VT100_ASCII_ESC: /* ESCAPE */
                state = SHELL_RECEIVE_ESC;
                break;

            case '\0':
            case '\t': /* TAB */
                break;

            case SHELL_VT100_ASCII_BSPACE: /* BACKSPACE */
                if(idx==0)
                    continue;

                memmove(&shell_buf[idx], &shell_buf[idx+1], strlen(&shell_buf[idx+1])+1);
                shell_buf[--idx] = 0;
                break;

            case SHELL_VT100_ASCII_DEL: /* DELETE */
                if(!shell_buf[idx+1])
                    continue;
                
                memmove(&shell_buf[idx+1], &shell_buf[idx+2], strlen(&shell_buf[idx+2])+1);
                break;

            default:
                if (isprint(data[i]))
                {
                    shell_buf[idx++] = data[i];
                    memmove(&shell_buf[idx+1], &shell_buf[idx], strlen(&shell_buf[idx]));
                    shell_buf[idx] = 0;
                }
                else
                {
                    // LOGI(TAG, "can't print");
                    ctrl_metakeys_handle(data[i]);
                }
                break;
            }
            break;

        case SHELL_RECEIVE_ESC:
            if (data[i] == '[')
            {
                state = SHELL_RECEIVE_ESC_SEQ;
                break;
            }
            state = SHELL_RECEIVE_DEFAULT;
            break;

        case SHELL_RECEIVE_ESC_SEQ:
            state = SHELL_RECEIVE_DEFAULT;

            switch (data[i])
            {
            case 'A': /* UP arrow */
                memcpy(shell_buf, shell_his[h_idx], sizeof(shell_buf));
                idx = strlen(shell_buf);

                h_idx++;
                if (h_idx >= SHELL_HIS_LEN)
                {
                    h_idx = SHELL_HIS_LEN - 1;
                }
                break;
            case 'B': /* DOWN arrow */
                if (h_idx)
                {
                    h_idx--;
                }

                memcpy(shell_buf, shell_his[h_idx], sizeof(shell_buf));
                idx = strlen(shell_buf);
                break;

            case 'C': /* RIGHT arrow */
                if(!shell_buf[idx+1])
                    continue;

                shell_buf[idx] = shell_buf[idx+1];
                shell_buf[++idx] = 0;
                break;

            case 'D': /* LEFT arrow */
                if (idx == 0)
                    continue;
                
                shell_buf[idx] = shell_buf[idx-1];
                shell_buf[--idx] = 0;
                break;

            case '3': /* DELETE Button in ESC[n~ mode */
                state = SHELL_RECEIVE_TILDE_EXP;
                if(!shell_buf[idx+1])
                    continue;
                
                memmove(&shell_buf[idx+1], &shell_buf[idx+2], strlen(&shell_buf[idx+2])+1);
                break;
            default:
                break;
            }
            break;

        case SHELL_RECEIVE_TILDE_EXP:
        default:
            state = SHELL_RECEIVE_DEFAULT;
            break;
        }
    }  

    // clear line
    printf("\33[2K\r");
    printf("%s%s%s", "mcu-cli > ", &shell_buf[0], &shell_buf[idx+1]);
    fflush(stdout);
    for (size_t i = 0; i < strlen(&shell_buf[idx+1]); i++)
        printf("\b");
    fflush(stdout);
}


#include <zephyr/kernel.h>

static int execute_shell(int argc, char *argv[], struct shell_t shell_list[]) {
    for (size_t i = 0; shell_list[i].name != NULL; i++) {
        if ( !strcasecmp(argv[0], shell_list[i].name)) {
            if (shell_list[i].sub && argc >= 1) {
                return execute_shell(argc-1, argv+1, shell_list[i].sub);
            }

            if (shell_list[i].func) {
                return shell_list[i].func(argc-1, argv+1);
            }
        }
    }
    
    return -EINVAL;
}

__attribute__((weak)) int parse_shell(uint8_t *shell, uint32_t len) {
    (void) len;
    int ret = 0;

    int argc = 0;
    char *saveptr;
    char *argv[20] = {NULL};

    // parse shell
    argv[argc] = strtok_r((char *)shell, " ", &saveptr);
    LOG_DBG("%s", argv[argc]);
    
    while(argv[argc] != NULL) {
        argc++;

        // check ovrflow
        if (argc > ARRAY_SIZE(argv)-1 ) { 
            break; 
        }

        argv[argc] = strtok_r(NULL, " ", &saveptr);
        LOG_DBG("%s", argv[argc]);
    }

    // check shell keyword
    extern struct shell_t _shell_t_list_start[];
    ret = execute_shell(argc, argv, _shell_t_list_start);
    switch (ret) {
        case 0:
            break;
        case -EINVAL:
            LOGE(TAG, "Invalid argument");
            break;
        
        default:
            LOGE(TAG, "Unknow: %d", ret);
            break;
    }
    
    return 0;
}


static uint8_t tab_idx = 0;
static char tab[10] = {0};
static void shell_dump(struct shell_t shell_list[]) {
    for (size_t i = 0; shell_list[i].name != NULL; i++) {
        if (shell_list[i].sub) {
            LOGI(TAG, "%s%s: %s", tab, shell_list[i].name,
                 shell_list[i].info);

            tab[tab_idx++] = '\t';
            shell_dump(shell_list[i].sub);
            tab[--tab_idx] = '\0';
        } else {
            LOGI(TAG, "%s%s - %s", tab, shell_list[i].name,
                 shell_list[i].info);
        }
    }
}

static int shell_help(int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    extern struct shell_t _shell_t_list_start[];
    shell_dump(_shell_t_list_start);

    return 0;
}

STRUCT_SECTION_ITERABLE(shell_t, help) = {
    .name = "help",
    .info = "list shell",
    .func = shell_help,
    .sub = NULL,
};

STRUCT_SECTION_ITERABLE(shell_t, zz_end) = {
    .name = NULL,
    .info = NULL,
    .func = NULL,
    .sub = NULL,
};
