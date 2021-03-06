/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017/10/15     bernard      implement stdio for IAR dlib.
 */
#include <rtthread.h>

#ifdef RT_USING_POSIX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "libc.h"

#define STDIO_DEVICE_NAME_MAX   32

static int std_fd = -1;
int libc_stdio_set_console(const char* device_name, int mode)
{
    int fd;
    char name[STDIO_DEVICE_NAME_MAX];

    snprintf(name, sizeof(name) - 1, "/dev/%s", device_name);
    name[STDIO_DEVICE_NAME_MAX - 1] = '\0';

    fd = open(name, mode, 0);
    if (fd >= 0)
    {
        if (std_fd >= 0)
        {
            close(std_fd);
        }
        std_fd = fd;
    }

    return std_fd;
}

int libc_stdio_get_console(void) {
    return std_fd;
}

#endif /* RT_USING_POSIX */
