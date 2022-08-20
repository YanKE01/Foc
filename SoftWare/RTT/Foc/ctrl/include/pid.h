/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-12     SkyLin       the first version
 */
#ifndef CTRL_INCLUDE_PID_H_
#define CTRL_INCLUDE_PID_H_



class Pid{
public:
    Pid(float kp,float kd,float ki);
    float Realize(float now,float target);
    float constrain(float x,float low,float high);
    void Change(float kp,float kd,float ki);
private:
    float kp,kd,ki;
    float err,errLast,errLastLast;
    float iPrev;
    float out;
    float Ts=0.001f;

};


extern Pid speedPid;

#endif /* CTRL_INCLUDE_PID_H_ */
