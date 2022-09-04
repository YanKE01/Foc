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


__STATIC_INLINE uint32_t GXT_SYSTICK_IsActiveCounterFlag(void)
{
  return ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == (SysTick_CTRL_COUNTFLAG_Msk));
}

static uint32_t getCurrentMicros(void)
{
  /* Ensure COUNTFLAG is reset by reading SysTick control and status register */
  GXT_SYSTICK_IsActiveCounterFlag();
  uint32_t m = HAL_GetTick();
  const uint32_t tms = SysTick->LOAD + 1;
  __IO uint32_t u = tms - SysTick->VAL;
  if (GXT_SYSTICK_IsActiveCounterFlag()) {
    m = HAL_GetTick();
    u = tms - SysTick->VAL;
  }
  return (m * 1000 + (u * 1000) / tms);
}
//获取系统时间，单位us
uint32_t micros(void)
{
  return getCurrentMicros();
}

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
    rt_pin_mode(this->hall_u, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(this->hall_v, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(this->hall_w, PIN_MODE_INPUT_PULLUP);
    direction = Direction::UNKNOWN; //默认为未知状态
    cpr = polePairs * 6; //1极对有6中状态
    angleCurrent = anglePrev = 0.0f;
    velocity = 0.0f;

    //初始化时先读取一起状态
    uState = rt_pin_read(hall_u);
    vState = rt_pin_read(hall_v);
    wState = rt_pin_read(hall_w);

    UpdateHall();
    pulse_timestamp = micros();
}

/**
 * 更新霍尔数据
 */
void HallSensor::UpdateHall()
{
    long new_pulse_timestamp = micros();

    int8_t newHallState = wState + (vState << 1) + (uState << 2);
    if (newHallState == hallState)
    {
        return; //电机未转动
    }

    hallState = newHallState; //更新hall状态
    int newElectricSector = ELECTRIC_SECTORS[hallState]; //获取扇区
    static Direction old_direction;

    if (newElectricSector - electricSector > 3)
    {
        direction = Direction::CCW; //逆时针转动
        electricRotations += direction; //累加次数
    }
    else if (newElectricSector - electricSector < -3)
    {
        direction = Direction::CW; //顺时针转动
        electricRotations += direction; //累加次数
    }
    else
    {
        direction = newElectricSector > electricSector ? Direction::CW : Direction::CCW;
    }

    electricSector = newElectricSector; //更新扇区

    if (direction == old_direction)
    {
        //方向保持一致
        pulse_diff = new_pulse_timestamp - pulse_timestamp;
    }
    else
    {
        pulse_diff = 0;
    }

    pulse_timestamp = new_pulse_timestamp;
    old_direction = direction;

}

float HallSensor::GetAngle()
{
    return ((float) (electricRotations * 6 + electricSector) / (float) cpr) * _2PI;

}

void HallSensor::GetDirection()
{
    rt_kprintf("Dir:%d,Sector:%d\n", direction, electricSector);
}

float HallSensor::GetSpeed()
{
    if (pulse_diff == 0 || ((long) (micros() - pulse_timestamp) > pulse_diff))
    {
        return 0;
    }
    else
    {
        return direction * (_2PI / (float) cpr) / (pulse_diff / 1000000.0f); //TS=1us
    }

}

float HallSensor::GetSpeedLpf()
{
    float velocity = GetSpeed();
    float y = 0.9f * velocity_prev + 0.1f * velocity;
    velocity_prev = velocity_prev;

    return y;
}

/**
 * 为IO中断服务
 */
void HallSensor::UpdateU()
{
    uState = rt_pin_read(hall_u);
    UpdateHall();
}

void HallSensor::UpdateV()
{
    vState = rt_pin_read(hall_v);
    UpdateHall();
}

void HallSensor::UpdateW()
{
    wState = rt_pin_read(hall_w);
    UpdateHall();
}

rt_base_t HallSensor::GetUPin()
{
    return hall_u;
}

rt_base_t HallSensor::GetVPin()
{
    return hall_v;
}

rt_base_t HallSensor::GetWPin()
{
    return hall_w;
}

HallSensor::~HallSensor()
{
    // TODO 自动生成的析构函数存根
}

