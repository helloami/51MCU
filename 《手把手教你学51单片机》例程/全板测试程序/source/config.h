/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套Kingst51单片机开发板 示例源代码
*
*         (c) 版权所有 2025 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：config.h
* 描  述：全板测试程序 工程配置头文件
* 版本号：v1.0.0
* 备  注：
*******************************************************************************
*/

#ifndef _CONFIG_H
#define _CONFIG_H

/* 通用头文件 */
#include <reg52.h>
#include <intrins.h>

/* 数据类型定义 */
typedef  signed    char    int8;    // 8位有符号整型数
typedef  signed    int     int16;   //16位有符号整型数
typedef  signed    long    int32;   //32位有符号整型数
typedef  unsigned  char    uint8;   // 8位无符号整型数
typedef  unsigned  int     uint16;  //16位无符号整型数
typedef  unsigned  long    uint32;  //32位无符号整型数

/* 全局运行参数定义 */
#define SYS_MCLK   (11059200/12)  //系统主时钟频率，即晶振频率÷12

/* IO引脚分配定义 */
sbit KEY_IN_1  = P2^4;  //矩阵按键的扫描输入引脚1
sbit KEY_IN_2  = P2^5;  //矩阵按键的扫描输入引脚2
sbit KEY_IN_3  = P2^6;  //矩阵按键的扫描输入引脚3
sbit KEY_IN_4  = P2^7;  //矩阵按键的扫描输入引脚4
sbit KEY_OUT_1 = P2^3;  //矩阵按键的扫描输出引脚1
sbit KEY_OUT_2 = P2^2;  //矩阵按键的扫描输出引脚2
sbit KEY_OUT_3 = P2^1;  //矩阵按键的扫描输出引脚3
sbit KEY_OUT_4 = P2^0;  //矩阵按键的扫描输出引脚4

sbit ADDR0 = P1^0;  //LED位选译码地址引脚0
sbit ADDR1 = P1^1;  //LED位选译码地址引脚1
sbit ADDR2 = P1^2;  //LED位选译码地址引脚2
sbit ADDR3 = P1^3;  //LED位选译码地址引脚3
sbit ENLED = P1^4;  //LED显示部件的总使能引脚

sbit SPI_SCK  = P3^5;  //SPI总线时钟引脚
sbit SPI_MISO = P3^4;  //SPI总线主入从出引脚
sbit SPI_MOSI = P1^5;  //SPI总线主出从入引脚
sbit SPI_SSEL = P1^7;  //SPI总线从机选择引脚

sbit I2C_SCL = P3^7;  //I2C总线时钟引脚
sbit I2C_SDA = P3^6;  //I2C总线数据引脚

sbit BUZZ  = P1^6;  //蜂鸣器控制引脚
sbit RELAY = P3^3;  //继电器控制引脚

sbit IO_18B20 = P3^2;  //DS18B20通信引脚


#endif
