/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 * 命名规范：https://zh-google-styleguide.readthedocs.io/en/latest/google-cpp-styleguide/naming/
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-02     RT-Thread    first version
 */

#include <rtthread.h>
#include <board.h>
#include <rtdevice.h>
#include "include/task.h"
#include "tim.h"
int main(void)
{
    MX_TIM1_Init();
    TaskStartUp();
    return RT_EOK;
}
