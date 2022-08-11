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
    MX_TIM1_Init();
}
