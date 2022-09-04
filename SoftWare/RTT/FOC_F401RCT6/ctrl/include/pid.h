/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-04     SkyLin       the first version
 */
#ifndef CTRL_PID_H_
#define CTRL_PID_H_

class Pid
{
public:
    Pid()=default;
    Pid(float kp,float ki,float kd);
    float Realize(float now,float target);
    float constrain(float x,float low,float high);
    void Change(float kp,float kd,float ki);
    ~Pid();

private:
    float kp,ki,kd;
    float err,errLast,errLastLast;
    float iPrev;
    float out;
    float Ts=0.001f;
};

#endif /* CTRL_PID_H_ */
