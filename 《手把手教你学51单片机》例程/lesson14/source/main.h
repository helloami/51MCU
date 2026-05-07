/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套Kingst51单片机开发板 示例源代码
*
*         (c) 版权所有 2025 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.h
* 描  述：第14章 温度控制系统 工程主文件的头文件
* 版本号：v1.0.0
* 备  注：
*******************************************************************************
*/

#ifndef _MAIN_H
#define _MAIN_H

#include "config.h"

/* 温度相关参数，温度数值左移4位是为与DS18B20数据格式保持一致 */
#define ACT_TEMP_ADDR         0x30   //继电器动作温度的E2存储地址
#define ACT_TEMP_MIN        (20<<4)  //继电器动作温度有效范围最小值
#define ACT_TEMP_MAX        (30<<4)  //继电器动作温度有效范围最大值
#define ACT_TEMP_DEFAULT    (25<<4)  //继电器动作温度默认值
#define ALARM_TEMP_ADDR       0x32   //高温报警温度的E2存储地址
#define ALARM_TEMP_MIN      (25<<4)  //高温报警温度有效范围最小值
#define ALARM_TEMP_MAX      (35<<4)  //高温报警温度有效范围最大值
#define ALARM_TEMP_DEFAULT  (30<<4)  //高温报警温度默认值

void TempControl();
void KeyAction(uint8 keycode);
void ConfigTimer0(uint16 ms);

#endif
