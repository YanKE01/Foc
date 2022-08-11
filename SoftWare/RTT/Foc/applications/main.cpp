/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-11     RT-Thread    first version
 */

#include <rtthread.h>
#include "tim.h"

int main(void)
{
    MX_TIM1_Init();

    return RT_EOK;
}
