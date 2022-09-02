/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-02     SkyLin       the first version
 */
#ifndef CTRL_HALL_SENSOR_H_
#define CTRL_HALL_SENSOR_H_

#include <rtthread.h>
#include <rtdevice.h>
#include <stdint.h>

//电机转动时对应的hall输出与扇区的对应
const int8_t ELECTRIC_SECTORS[8] = { -1,  0,  4,  5,  2,  1,  3 , -1 };

enum Direction : int8_t {
    CW      = 1,  // 顺时针
    CCW     = -1, // 逆时针
    UNKNOWN = 0   // 未知
};

class HallSensor
{
public:
    HallSensor() = default;
    HallSensor(rt_base_t hall_u, rt_base_t hall_v, rt_base_t hall_w,int polePairs);
    void UpdateHall();//更新Hall传感器相关数据
    ~HallSensor();
private:
    rt_base_t hall_u,hall_v,hall_w;
    int polePairs;//极对数
    int hallState;//霍尔传感器当前状态
    int electricSector;//电扇区
    Direction direction;//转向
};

#endif /* CTRL_HALL_SENSOR_H_ */
