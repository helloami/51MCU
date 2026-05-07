/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套Kingst51单片机开发板 示例源代码
*
*         (c) 版权所有 2025 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：Led.h
* 描  述：点阵LED、数码管、独立LED驱动模块头文件
* 版本号：v1.0.0
* 备  注：
*******************************************************************************
*/

#ifndef _LED_H
#define _LED_H

#include "config.h"

struct sLedBuff {  //LED显示缓冲区结构
    uint8 array[8];   //点阵缓冲区
    uint8 number[6];  //数码管缓冲区
    uint8 single;     //独立LED缓冲区
};

void InitLed();
void LedScan();
void FlowingLight();
void TestLedArray();
void ShowTempValue(int16 temp);
void LedNumber(uint8 index, uint8 num, uint8 point);

#endif
