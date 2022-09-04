/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-03     SkyLin       the first version
 */
#include "foc.h"
#include "tim.h"
#include <math.h>
#include "arm_math.h"

/**
 * @brief Construct a new Foc:: Foc object
 *
 * @param voltagePowerSupply 母线电压
 */
Foc::Foc(float voltagePowerSupply)
{
    this->voltagePowerSupply = voltagePowerSupply;
}

/**
 * @brief 归一化角度
 *
 * @param angle
 * @return float
 */
float Foc::NormalizeAngle(float angle)
{
    float a = fmod(angle, _2PI);
    return a >= 0 ? a : (a + _2PI);
}

void Foc::ParkAntiOperate(float ud, float uq, float theta)
{
    ualpha = ud * arm_cos_f32(theta) - uq * arm_sin_f32(theta);
    ubeta = ud * arm_sin_f32(theta) + uq * arm_cos_f32(theta);
}


/**
 * @brief SVPWM计算
 * 
 * @param uq 
 * @param ud 
 * @param angleEl 
 */
void Foc::Svpwm(float uq, float ud, float angleEl)
{
    if (uq < 0)
    {
        angleEl = NormalizeAngle(angleEl - _PI_2); //顺时针
    }
    else
    {
        angleEl = NormalizeAngle(angleEl + _PI_2); //逆时针
    }

    // 1-反park变换
    ParkAntiOperate(ud, uq, angleEl);

    // 2-计算参考电压矢量，限制其在0~1,并且为保证最大不失真 需要限制参考矢量在内切圆中 (根号3)/3
    uref = sqrt(ualpha * ualpha + ubeta * ubeta) / voltagePowerSupply;
    if (uref > 0.577f)
    {
        uref = 0.577f;
    }
    if (uref < -0.577f)
    {
        uref = -0.577f;
    }

    // 3-计算所在扇区
    sector = (angleEl / _PI_3) + 1;

    // 4-计算相邻矢量的作用时间，此处参考simplefoc
    t1 = _SQRT3 * arm_sin_f32(sector * _PI_3 - angleEl) * uref;
    t2 = _SQRT3 * arm_sin_f32(angleEl - (sector - 1.0) * _PI_3) * uref;
    t0 = 1 - t1 - t2;

    // 5-根据7段PWM计算每一项的作用时间
    switch (sector)
    {
    case 1:
        ta = t1 + t2 + t0 / 2;
        tb = t2 + t0 / 2;
        tc = t0 / 2;
        break;
    case 2:
        ta = t1 + t0 / 2;
        tb = t1 + t2 + t0 / 2;
        tc = t0 / 2;
        break;
    case 3:
        ta = t0 / 2;
        tb = t1 + t2 + t0 / 2;
        tc = t2 + t0 / 2;
        break;
    case 4:
        ta = t0 / 2;
        tb = t1 + t0 / 2;
        tc = t1 + t2 + t0 / 2;
        break;
    case 5:
        ta = t2 + t0 / 2;
        tb = t0 / 2;
        tc = t1 + t2 + t0 / 2;
        break;
    case 6:
        ta = t1 + t2 + t0 / 2;
        tb = t0 / 2;
        tc = t1 + t0 / 2;
        break;
    default:
        ta = 0;
        tb = 0;
        tc = 0;
        break;
    }

    ccr1 = 100 * ta; // 100是arr的值
    ccr2 = 100 * tb; // 100是arr的值
    ccr3 = 100 * tc; // 100是arr的值

    TIM1->CCR1 = ccr1;
    TIM1->CCR2 = ccr2;
    TIM1->CCR3 = ccr3;
}

Foc::~Foc()
{
}
