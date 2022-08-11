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
#include "hal.h"
#include "task.h"
#include "encoder.h"
int main(void)
{
    HalInit();
    TaskInit();

    return RT_EOK;
}
