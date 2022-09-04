/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-03     SkyLin       the first version
 */
#include "include/task.h"
#include <memory>


/***********线程参数设置区域****************/
static char flashLedTaskStack[512]; /*!< 定义呼吸灯线程堆栈 */
static struct rt_thread flashLedTask; /*!< 定义呼吸灯线程 */
//static char sensorStateUpdateTaskStack[2048]; /*!< 定义传感器状态更新线程堆栈 */
//static struct rt_thread sensorStateUpdateTask; /*!< 定义传感器状态更新线程 */
static char uiTaskStack[2048]; /*!< 定义界面堆栈 */
static struct rt_thread uiTask; /*!< 定义界面线程 */
static char focTaskStack[2048]; /*!< 定义FOC堆栈 */
static struct rt_thread focTask; /*!< 定义FOC线程 */


HallSensor hall(GET_PIN(B,0),GET_PIN(B,5),GET_PIN(B,4),2);
Lcd lcd("spi10",GET_PIN(C,0),GET_PIN(C,2));
Foc foc(24.0f);
float anglec,anglep,vel;

/****************自定义函数**********************/
void UpdateHallU(void *args)
{
    hall.UpdateU();
}

void UpdateHallV(void *args)
{
    hall.UpdateV();
}

void UpdateHallW(void *args)
{
    hall.UpdateW();
}

void EnableHallIrq()
{
    rt_pin_attach_irq(hall.GetUPin(), PIN_IRQ_MODE_RISING_FALLING ,UpdateHallU, RT_NULL);
    rt_pin_irq_enable(hall.GetUPin(), PIN_IRQ_ENABLE);

    rt_pin_attach_irq(hall.GetVPin(), PIN_IRQ_MODE_RISING_FALLING ,UpdateHallV, RT_NULL);
    rt_pin_irq_enable(hall.GetVPin(), PIN_IRQ_ENABLE);

    rt_pin_attach_irq(hall.GetWPin(), PIN_IRQ_MODE_RISING_FALLING ,UpdateHallW, RT_NULL);
    rt_pin_irq_enable(hall.GetWPin(), PIN_IRQ_ENABLE);
}

/***********线程入口区域****************/
void FlashLedTask_Entry(void *pvParameter)
{
    rt_pin_mode(GET_PIN(B,3), PIN_MODE_OUTPUT);
    rt_uint8_t state=0;
    while(1)
    {
        state=!state;
        rt_pin_write(GET_PIN(B,3), state);
        rt_thread_mdelay(1000);
    }
}


void UiTask_Entry(void *pvParameter)
{
    lcd.Init();
    char lcdBuf[100];
    lcd.ShowStr(0, 0, "Foc Monitor");
    while(1)
    {
        rt_sprintf(lcdBuf, "speed:%.2f",vel);
        lcd.ShowStr(0, 2, lcdBuf);
        rt_thread_mdelay(100);
    }
}




void FocOperate_Entry(void *pvParameter)
{
    float angleEl=0.0f;

    EnableHallIrq();//使能IO中断
    while(1)
    {
        angleEl=hall.GetAngle();
        angleEl=angleEl*2.0f;
        foc.Svpwm(2, 0, angleEl);
        rt_thread_mdelay(1);
    }
}
void TaskStartUp()
{
    /*<-呼吸灯线程初始化->*/
    rt_thread_init(&flashLedTask, "flashLed", FlashLedTask_Entry, RT_NULL, &flashLedTaskStack[0], sizeof(flashLedTaskStack), 15, 10);
    rt_thread_startup(&flashLedTask);

    rt_thread_init(&uiTask, "ui", UiTask_Entry, RT_NULL, &uiTaskStack[0], sizeof(uiTaskStack), 10, 10);
    rt_thread_startup(&uiTask);

    rt_thread_init(&focTask, "foc", FocOperate_Entry, RT_NULL, &focTaskStack[0], sizeof(focTaskStack), 5, 10);
    rt_thread_startup(&focTask);
}


