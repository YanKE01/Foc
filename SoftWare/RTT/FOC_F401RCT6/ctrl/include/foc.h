/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-03     SkyLin       the first version
 */
#ifndef CTRL_FOC_H_
#define CTRL_FOC_H_

#include "stdint.h"
#include "rtdef.h"


#define _2_SQRT3 1.15470053838f
#define _SQRT3 1.73205080757f
#define _1_SQRT3 0.57735026919f
#define _SQRT3_2 0.86602540378f
#define _SQRT2 1.41421356237f
#define _120_D2R 2.09439510239f
#define _PI 3.14159265359f
#define _PI_2 1.57079632679f
#define _PI_3 1.0471975512f
#define _2PI 6.28318530718f
#define _3PI_2 4.71238898038f
#define _PI_6 0.52359877559f

class Foc
{
public:
    Foc()=default;
    Foc(float voltagePowerSupply);
    void Svpwm(float uq,float ud,float angleEl);
    float NormalizeAngle(float angle);
    void ParkAntiOperate(float ud,float uq,float theta);
    ~Foc();
private:
    float ualpha;
    float ubeta;
    float uref;//参考电压矢量
    float voltagePowerSupply;
    uint8_t sector;
    float t1,t2,t0;//各扇区相邻矢量作用时间
    float ta,tb,tc;//每一项的作用时间
    uint32_t ccr1,ccr2,ccr3;
};
#endif /* CTRL_FOC_H_ */
