/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-11     SkyLin       the first version
 */
#ifndef HAL_ENCODER_H_
#define HAL_ENCODER_H_

#include <rtthread.h>
#include <rtdevice.h>
#include <string>

#define AS5600_RESOLUTION 4096
#define AS5600_RAW_ADDR    0x36
#define AS5600_RAW_ANGLE_REGISTER  0x0C
#define abs(x) ((x)>0?(x):-(x))
#define _2PI 6.28318530718


class Encoder
{
public:
    Encoder(std::string name);
    rt_uint16_t GetRawData(void);
    rt_err_t I2CWriteCmd(rt_uint8_t cmd);
    rt_err_t I2CReadRegs(rt_uint8_t *buf, rt_uint8_t length);
    float GetAngle();
    ~Encoder();

private:
    struct rt_i2c_bus_device *i2c1;
    rt_uint16_t rawData; //原始数据
    struct rt_i2c_msg msg1, msg2;
    rt_uint8_t cmd;
    rt_uint8_t rawBuffer[2]; //存放0x0c的两个字节的数据
    float anglePrev; //上一次的角度值
    float angleErr; //角度差值
    float fullRotation;//转过的整数圈
};

extern Encoder as5600;

#endif /* HAL_ENCODER_H_ */
