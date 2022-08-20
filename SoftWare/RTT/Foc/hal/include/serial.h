/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-12     SkyLin       the first version
 */
#ifndef HAL_INCLUDE_SERIAL_H_
#define HAL_INCLUDE_SERIAL_H_

#include <rtthread.h>
#include <rtdevice.h>

void SerialInit();
void SerialWrite(char *data);



#endif /* HAL_INCLUDE_SERIAL_H_ */
