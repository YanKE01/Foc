/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-02     SkyLin       the first version
 */
#include "hall_sensor.h"

/**
 * Hall构造
 * @param hall_u
 * @param hall_v
 * @param hall_w
 * @param polePairs:极对数
 */
HallSensor::HallSensor(rt_base_t hall_u, rt_base_t hall_v, rt_base_t hall_w, int polePairs)
{
    this->hall_u = hall_u;
    this->hall_v = hall_v;
    this->hall_w = hall_w;
    this->polePairs = polePairs;
    rt_pin_mode(this->hall_u, PIN_MODE_INPUT);
    rt_pin_mode(this->hall_v, PIN_MODE_INPUT);
    rt_pin_mode(this->hall_w, PIN_MODE_INPUT);
    direction = Direction::UNKNOWN; //默认为未知状态

}

void HallSensor::UpdateHall()
{
    int8_t newHallState = rt_pin_read(hall_w) + (rt_pin_read(hall_v) << 1) + (rt_pin_read(hall_u) << 2);

    if (newHallState == hallState)
    {
        return; //电机未转动
    }

    hallState = newHallState; //更新hall状态
    int newElectricSector = ELECTRIC_SECTORS[hallState]; //获取扇区

    if (newElectricSector - electricSector > 3)
    {
        direction = Direction::CCW; //逆时针转动
    }
    else if (newElectricSector - electricSector < -3)
    {
        direction = Direction::CW; //顺时针转动
    }
    else
    {
        direction = newElectricSector > electricSector ? Direction::CW : Direction::CCW;
    }

    electricSector=newElectricSector;//更新扇区
}

HallSensor::~HallSensor()
{
    // TODO 自动生成的析构函数存根
}

