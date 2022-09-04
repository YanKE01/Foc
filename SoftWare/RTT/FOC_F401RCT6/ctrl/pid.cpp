/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-04     SkyLin       the first version
 */
#include "pid.h"

Pid::Pid(float kp,float ki,float kd):kp(kp),ki(ki),kd(kd)
{

}


float Pid::constrain(float x, float low, float high)
{
    if (x < low)
    {
        x = low;
    }

    if (x > high)
    {
        x = high;
    }

    return x;
}


float Pid::Realize(float now, float target)
{
    float proportional,integral,derivative;

    err=target-now;
    proportional=kp*err;
    integral=iPrev+ki*Ts*err;
    derivative=kd*(err-errLast)/Ts;

    out=proportional+integral+derivative;
    out=constrain(out, -10, 10);

    iPrev=integral;
    errLast=err;

    return out;
}

Pid::~Pid()
{
}

