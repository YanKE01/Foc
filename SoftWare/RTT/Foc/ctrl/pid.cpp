/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-12     SkyLin       the first version
 */
#include "pid.h"
#include <rtthread.h>
#include "stdlib.h"
#include "task.h"

Pid speedPid(0.15,1.1,0.0);
extern float targetSpeed;

Pid::Pid(float kp, float ki, float kd)
{
   this->kp=kp;
   this->ki=ki;
   this->kd=kd;
   out=0;
   err=errLast=errLastLast=0;
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
    out=constrain(out, -6, 6);

    iPrev=integral;
    errLast=err;

    return out;
}

void Pid::Change(float kp,float ki,float kd)
{
    this->kd=kd;
    this->ki=ki;
    this->kp=kp;

    rt_kprintf("Pid Para:%.2f,%.2f,%.2f\n",this->kp,this->ki,this->kd);
}

static void pidset(int argc, char *argv[])
{
    if(argc==4)
    {
        speedPid.Change(atof(argv[1]), atof(argv[2]), atof(argv[3]));
    }
}


static void speed(int argc, char *argv[])
{
    float test=0;
    if(argc==2)
    {
        test=atof(argv[1]);
        if(test>0 && test<90)
        {
            targetSpeed=test;
            rt_kprintf("speed:%.2f\n",targetSpeed);
        }
    }
}


MSH_CMD_EXPORT(pidset, pidset);
MSH_CMD_EXPORT(speed, speed);


