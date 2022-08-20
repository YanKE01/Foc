/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-11     SkyLin       the first version
 */
#include "hal.h"


/**
* @brief  外设初始化
* @param  None
* @return None
* @retval None
*/
void HalInit()
{
    rt_pin_mode(ledFlash,PIN_MODE_OUTPUT);//led初始化
    rt_pin_mode(userButton, PIN_MODE_INPUT_PULLUP);//用户按键初始化
    rt_pin_mode(drvEn, PIN_MODE_OUTPUT);//驱动使能引脚
    SerialInit();
    MX_TIM1_Init();
}
