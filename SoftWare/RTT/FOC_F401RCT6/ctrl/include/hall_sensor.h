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

extern "C"
{
    #include <rtthread.h>
    #include <rtdevice.h>
    #include <stdint.h>
    #include "board.h"
}
#include <memory>


#define _2PI 6.28318530718f

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
    void GetDirection();
    float GetAngle();
    float GetSpeed();//获取转速
    float GetSpeedLpf();//获取转速-低通
    void UpdateU();
    void UpdateV();
    void UpdateW();
    rt_base_t GetUPin();
    rt_base_t GetVPin();
    rt_base_t GetWPin();
    ~HallSensor();
private:
    rt_base_t hall_u,hall_v,hall_w;
    int polePairs;//极对数
    int cpr;//转一圈对应的状态变化次数
    int hallState;//霍尔传感器当前状态
    int electricSector;//电扇区
    Direction direction;//转向
    volatile long electricRotations;//转动次数
    float velocity;
    float velocity_prev;
    float angleCurrent,anglePrev;
    int uState,vState,wState;//u,v,w状态
    volatile long pulse_diff;
    volatile unsigned long pulse_timestamp;

};



#endif /* CTRL_HALL_SENSOR_H_ */
