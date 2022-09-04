/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-03     SkyLin       the first version
 */
#ifndef APPLICATIONS_INCLUDE_TASK_H_
#define APPLICATIONS_INCLUDE_TASK_H_

#include "hall_sensor.h"
#include "hal/include/lcd.h"
#include "foc.h"

extern "C"{
    #include <rtdevice.h>
    #include <board.h>
    #include <rtthread.h>
    #include "stdio.h"
}
void TaskStartUp();

#endif /* APPLICATIONS_INCLUDE_TASK_H_ */
