/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-11     SkyLin       the first version
 */
#include <encoder.h>

#define _2PI 6.28318530718

Encoder::Encoder()
{

}

/**
 * @brief  编码器构造函数
 * @param  i2c的名称
 * @return NULL
 */
Encoder::Encoder(std::string name)
{
    i2c1 = (struct rt_i2c_bus_device *) rt_device_find(name.c_str());

    if (i2c1 != RT_NULL)
    {
        rt_kprintf("I2C1 Register Success\r\n");
    }

}

/**
 * @brief  获取编码器原始数据
 * @param  NULL
 * @return 原始数据
 */
rt_uint16_t Encoder::GetRawData(void)
{
    rt_uint8_t cmd = AS5600_RAW_ANGLE_REGISTER;
    I2CWriteCmd(cmd);
    I2CReadRegs(rawBuffer, 2);

    return ((rt_uint16_t) rawBuffer[0] << 8) | (rt_uint16_t) rawBuffer[1];
}

/**
 * @brief  I2C写指令
 * @param  指令
 * @return RT_EOK:写成功
 */
rt_err_t Encoder::I2CWriteCmd(rt_uint8_t cmd)
{
    msg1.addr = AS5600_RAW_ADDR;
    msg1.flags = RT_I2C_WR;
    msg1.buf = &cmd;
    msg1.len = 1;

    if (rt_i2c_transfer(i2c1, &msg1, 1) == 1)
    {
        return RT_EOK;
    }
    return -RT_ERROR;
}

/**
 * @brief  I2C读数据
 * @param  buf:存放返回数据 length:返回数据长度
 * @return RT_EOK:读取成功
 */
rt_err_t Encoder::I2CReadRegs(rt_uint8_t *buf, rt_uint8_t length)
{
    msg2.addr = AS5600_RAW_ADDR;
    msg2.flags = RT_I2C_RD;
    msg2.buf = buf;
    msg2.len = length;

    if (rt_i2c_transfer(i2c1, &msg2, 1) == 1)
    {
        return RT_EOK;
    }
    return -RT_ERROR;

}

/**
 * @brief  获取数据
 * @param  NULL
 * @return 转角
 */
float Encoder::GetAngle()
{
    float angle = GetRawData();
    angleErr = angle - anglePrev;

    if (abs(angleErr) > (0.8f * AS5600_RESOLUTION))
    {
        fullRotation += (angleErr > 0 ? -_2PI : _2PI);
    }

    anglePrev = angle;

    return -(fullRotation + (angle / (float) AS5600_RESOLUTION) * _2PI);
}

/**
 * @brief  低通滤波
 * @param  当前数据
 * @return 滤波后数据
 */
float Encoder::Lpf(float x)
{
    float y = 0.9f*speedPrev + 0.1f*x;

    speedPrev=y;

    return y;
}

/**
 * @brief  获取转速
 * @param  angleNow:当前角度 Ts:周期
 * @return 转角
 */
float Encoder::GetSpeed(float angleNow,float Ts)
{
    speed=(angleNow-speedAnglePrev)/Ts;
    speed=Lpf(speed);//滤波
    speedAnglePrev=angleNow;

    return speed;
}

Encoder::~Encoder()
{
}

Encoder as5600("i2c1"); //在栈上声明

