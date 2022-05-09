/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018/09/30     Bernard      The first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <stdio.h>
//#include <unistd.h> Ӧ�ò���Ҫ ����ɾ��
#include <stdlib.h>
#include "fpioa.h" //�ɱ���߼�����->BSP
#include "board_config.h"//BSP����
#include "plic.h"//PLIC ��ѭRISC-V��Ȩ�ܹ��淶������֧�����1023���ⲿ�ж�Դ�����15872�����������ġ�->BSP
#include "sysctl.h"//ϵͳ�������Ĵ���->BSP

#define INCBIN_STYLE INCBIN_STYLE_SNAKE
#define INCBIN_PREFIX

#define PLL0_OUTPUT_FREQ 800000000UL
#define PLL1_OUTPUT_FREQ 400000000UL



rt_thread_t t_Predict =NULL ;//����һ��predict-thread



extern void _thread_Predict(void *params);
static int Predict();//Used to represent global variables and functions that cannot be accessed by other files. Static means that the scope of functions is limited to this file only

int main(void)
{
    int store[3] = {222,587,133};
    sysctl_clock_enable(SYSCTL_CLOCK_AI);//The AI-related clock was enabled
    io_mux_init();//The IO related bus is enabled
    io_set_power();//Set power supply voltage
    printf("hello world~\n");
    Predict();
    return 0;
}

static int Predict()
{
    t_Predict = rt_thread_create("predictImage", _thread_Predict, NULL, 200*1024, 10, 10);
    if(!t_Predict) return -1;
    rt_kprintf("startup thread t_Predict\n");
    rt_thread_startup(t_Predict);
    return 0;
}


