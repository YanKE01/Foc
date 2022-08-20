/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-12     SkyLin       the first version
 */

#include "serial.h"
#include "string.h"

static rt_device_t serial6;
struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; /* 初始化配置参数 */

void SerialInit()
{
    serial6 = rt_device_find("uart6");

    if (!serial6)
    {
        rt_kprintf("find Uart6 failed\r\n");
        return;
    }

    config.baud_rate = BAUD_RATE_115200;
    config.data_bits = DATA_BITS_8;
    config.stop_bits = STOP_BITS_1;
    config.bufsz = 512;
    config.parity = PARITY_NONE;

    rt_device_control(serial6, RT_DEVICE_CTRL_CONFIG, &config);
    rt_device_open(serial6, RT_DEVICE_FLAG_INT_RX);

}


void SerialWrite(char *data)
{
    rt_device_write(serial6, 0, data, strlen(data));
}
