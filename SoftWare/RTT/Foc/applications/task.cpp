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
#include "stdio.h"

rt_thread_t ledTask=RT_NULL;
rt_thread_t focTask=RT_NULL;

float targetSpeed=50.0f;


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


void FocTask_Entry(void *pvParameters)
{
    float angle,agnleEl,velocity;
    float uq;
    char buffer[100];
    foc.Enable();
    while(1)
    {
        angle=as5600.GetAngle();
        agnleEl=angle*7;

        uq=speedPid.Realize(velocity, targetSpeed);
        foc.Svpwm(uq,0,agnleEl);
        velocity=as5600.GetSpeed(angle, 0.001f);

        sprintf(buffer, "P:%d,%d,%d\n",(int)velocity,(int)targetSpeed,(int)uq);
        SerialWrite(buffer);

        rt_thread_mdelay(1);
    }
}

void TaskInit(void)
{
    ledTask=rt_thread_create("ledTask", LedTask_Entry, RT_NULL, 1024, 10, 10);
    if(ledTask != RT_NULL)
    {
        rt_thread_startup(ledTask);
    }

    focTask=rt_thread_create("focTask", FocTask_Entry, RT_NULL, 2048, 9, 10);
    if(focTask != RT_NULL)
    {
        rt_thread_startup(focTask);
    }
}




