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
#include "pid.h"

extern "C"
{
#include <rtdevice.h>
#include <board.h>
#include <rtthread.h>
#include "stdio.h"
#include "multi_button.h"
}


#define buttonRight GET_PIN(C,12)
#define buttonLeft GET_PIN(C,11)

void TaskStartUp();

#endif /* APPLICATIONS_INCLUDE_TASK_H_ */
