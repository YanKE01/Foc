/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-11     SkyLin       the first version
 */
#include "task.h"

rt_thread_t ledTask=RT_NULL;

/**
 * LED线程入口
 * @param pvParameters
 */
void LedTask_Entry(void *pvParameters)
{
    rt_uint8_t status=0;
    while(1)
    {
        status=!status;
        rt_pin_write(ledFlash, status);
        rt_thread_mdelay(500);
    }
}


void TaskInit(void)
{
    ledTask=rt_thread_create("ledTask", LedTask_Entry, RT_NULL, 1024, 10, 10);
    if(ledTask != RT_NULL)
    {
        rt_thread_startup(ledTask);
    }
}
