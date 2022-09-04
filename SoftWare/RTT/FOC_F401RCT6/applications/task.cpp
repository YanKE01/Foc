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

/***********线程参数设置区域****************/
static char flashLedTaskStack[512]; /*!< 定义呼吸灯线程堆栈 */
static struct rt_thread flashLedTask; /*!< 定义呼吸灯线程 */
static char hardwareUpdateTaskStack[2048]; /*!< 定义传感器状态更新线程堆栈 */
static struct rt_thread hardwareUpdateTask; /*!< 定义传感器状态更新线程 */
static char uiTaskStack[2048]; /*!< 定义界面堆栈 */
static struct rt_thread uiTask; /*!< 定义界面线程 */
static char focTaskStack[2048]; /*!< 定义FOC堆栈 */
static struct rt_thread focTask; /*!< 定义FOC线程 */

HallSensor hall(GET_PIN(B, 0), GET_PIN(B, 5), GET_PIN(B, 4), 2);
Lcd lcd("spi10", GET_PIN(C, 0), GET_PIN(C, 2));
Foc foc(24.0f);
Pid speedPid(0.2, 3.1, 0.0);
Pid anglePid(6.2f, 2.3f, 0.1f);

float uq = 0.0f;
float targetSpeed = 0.0f;
float targetAngle = 0.0f;
struct button rightBtn, leftBtn;
rt_uint8_t controlMode = 1; //默认速度控制模式0  位置模式为1

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

void ButtonLeftCb(void *btn)
{
    //减速
    uint32_t btn_event_val;

    btn_event_val = get_button_event((struct button *) btn);

    if (btn_event_val == SINGLE_CLICK)
    {
        if (controlMode == 0)
        {
            targetSpeed -= 1.0f; //速度-1
        }
        else if (controlMode == 1)
        {
            targetAngle -= 0.523f; //位置减少30°

        }
    }
}

/**
 * 右键回调
 * @param btn
 */
void ButtonRightCb(void *btn)
{
    //加速
    uint32_t btn_event_val;

    btn_event_val = get_button_event((struct button *) btn);

    switch (btn_event_val)
    {
    case SINGLE_CLICK:
        if (controlMode == 0)
        {
            targetSpeed += 1.0f; //速度-1
        }
        else if (controlMode == 1)
        {
            targetAngle += 0.523f; //位置减少30°
        }
        break;
    case DOUBLE_CLICK:
        controlMode = !controlMode;
        targetAngle=hall.GetAngle();
        targetSpeed=0.0f;
        lcd.Clear(BLACK);
        lcd.ShowStr(0, 0, "Foc Monitor,Version 0.1");

        break;
    default:
        break;
    }
}

void EnableHallIrq()
{
    rt_pin_attach_irq(hall.GetUPin(), PIN_IRQ_MODE_RISING_FALLING, UpdateHallU, RT_NULL);
    rt_pin_irq_enable(hall.GetUPin(), PIN_IRQ_ENABLE);

    rt_pin_attach_irq(hall.GetVPin(), PIN_IRQ_MODE_RISING_FALLING, UpdateHallV, RT_NULL);
    rt_pin_irq_enable(hall.GetVPin(), PIN_IRQ_ENABLE);

    rt_pin_attach_irq(hall.GetWPin(), PIN_IRQ_MODE_RISING_FALLING, UpdateHallW, RT_NULL);
    rt_pin_irq_enable(hall.GetWPin(), PIN_IRQ_ENABLE);
}

static uint8_t leftBtnRead(void)
{
    return rt_pin_read(GET_PIN(C, 11));
}

static uint8_t rightBtnRead(void)
{
    return rt_pin_read(GET_PIN(C, 12));
}

void MultiButton()
{
    rt_pin_mode(buttonLeft, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(buttonRight, PIN_MODE_INPUT_PULLUP);
    button_init(&leftBtn, leftBtnRead, PIN_LOW);
    button_init(&rightBtn, rightBtnRead, PIN_LOW);

    button_attach(&leftBtn, SINGLE_CLICK, ButtonLeftCb);
    button_start(&leftBtn);

    button_attach(&rightBtn, SINGLE_CLICK, ButtonRightCb);
    button_attach(&rightBtn, DOUBLE_CLICK, ButtonRightCb);

    button_start(&rightBtn);
}

/***********线程入口区域****************/
void FlashLedTask_Entry(void *pvParameter)
{
    rt_pin_mode(GET_PIN(B, 3), PIN_MODE_OUTPUT);
    rt_uint8_t state = 0;
    while (1)
    {
        state = !state;
        rt_pin_write(GET_PIN(B, 3), state);
        rt_thread_mdelay(1000);
    }
}

void UiTask_Entry(void *pvParameter)
{
    lcd.Init();
    char lcdBuf[100];
    lcd.ShowStr(0, 0, "Foc Monitor,Version 0.1");
    while (1)
    {
        rt_sprintf(lcdBuf, "speed:%.1f,uq:%.1f  ", hall.GetSpeedLpf(), uq);
        lcd.ShowStr(0, 2, lcdBuf);

        if (controlMode == 0)
        {
            rt_sprintf(lcdBuf, "TargetSpeed:%.1f   ", targetSpeed);
            lcd.ShowStr(0, 4, lcdBuf);
            lcd.ShowStr(0, 6, "Mode:Speed    ");
        }
        else if (controlMode == 1)
        {
            rt_sprintf(lcdBuf, "TargetPostion:%.1f       ", targetAngle * 57.296f);
            lcd.ShowStr(0, 4, lcdBuf);
            lcd.ShowStr(0, 6, "Mode:Position    ");
        }

        rt_thread_mdelay(1000);
    }
}

/**
 * 按键检测
 * @param pvParameter
 */
void HardwareUpdate_Entry(void *pvParameter)
{
    MultiButton();
    while (1)
    {
        button_ticks();
        rt_thread_mdelay(5);
    }
}

/**
 * FOC运算
 * @param pvParameter
 */
void FocOperate_Entry(void *pvParameter)
{
    float angleEl = 0.0f, angle = 0.0f;
    EnableHallIrq(); //使能IO中断
    while (1)
    {
        angle = hall.GetAngle();
        angleEl = angle * 2.0f;

        if (controlMode == 0)
        {
            uq = speedPid.Realize(hall.GetSpeedLpf(), targetSpeed);
            foc.Svpwm(uq, 0, angleEl);
        }
        else if (controlMode == 1)
        {
            uq = anglePid.Realize(angle, targetAngle);
            foc.Svpwm(uq, 0, angleEl);
        }

        rt_thread_mdelay(1);
    }
}

/**
 * 任务启动
 */
void TaskStartUp(void)
{
    /*<-呼吸灯线程初始化->*/
    rt_thread_init(&flashLedTask, "flashLed", FlashLedTask_Entry, RT_NULL, &flashLedTaskStack[0],
            sizeof(flashLedTaskStack), 15, 10);
    rt_thread_startup(&flashLedTask);

    /*<-UI线程初始化->*/
    rt_thread_init(&uiTask, "ui", UiTask_Entry, RT_NULL, &uiTaskStack[0], sizeof(uiTaskStack), 10, 10);
    rt_thread_startup(&uiTask);

    /*<-FOC线程初始化->*/
    rt_thread_init(&focTask, "foc", FocOperate_Entry, RT_NULL, &focTaskStack[0], sizeof(focTaskStack), 5, 10);
    rt_thread_startup(&focTask);

    /*<-硬件驱动更新线程->*/
    rt_thread_init(&hardwareUpdateTask, "hardware", HardwareUpdate_Entry, RT_NULL, &hardwareUpdateTaskStack[0],
            sizeof(hardwareUpdateTaskStack), 9, 10);
    rt_thread_startup(&hardwareUpdateTask);
}

