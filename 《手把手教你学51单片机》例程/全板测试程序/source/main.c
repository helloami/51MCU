/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套Kingst51单片机开发板 示例源代码
*
*         (c) 版权所有 2025 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：全板测试程序 工程主文件
* 版本号：v1.0.0
* 备  注：测试开发板上的各个部件
*******************************************************************************
*/

#include "DS18B20.h"
#include "keyboard.h"
#include "eeprom.h"
#include "flash.h"
#include "Led.h"
#include "Uart.h"
#include "main.h"

bit flag100ms = 0; //100ms定时标志
int16 curTemp = 0; //当前读取的温度值
uint8 tmrBeep = 0; //蜂鸣器鸣叫定时器
uint8 T0RH = 0;    //T0重载值的高字节
uint8 T0RL = 0;    //T0重载值的低字节

void main()
{
    uint8 tmr1s = 10;
    uint8 delay = 20;
    
    EA = 1;           //开总中断
    ConfigTimer0(1);  //配置T0定时1ms
    ConfigUART(9600); //配置串口通信
    InitLed();        //初始化LED模块
    Start18B20();     //启动首次温度转换
    BuzzBeep();       //蜂鸣器提示
    
    while (1)  //进入主循环
    {
        KeyDriver();      //执行按键驱动
        UartDriver();     //执行串口通信驱动
        if (flag100ms)    //每隔100ms执行以下分支
        {
            flag100ms = 0;
            if (delay > 0)
            {
                delay--;
                if (delay == 0)
                {   //本段代码仅在上电延时后执行一次
                    CheckE2rom();  //执行EEPROM的检测
                    CheckFlash();  //执行FLASH的检测
                }
                continue;
            }
            FlowingLight();  //流水灯效果实现
            TestLedArray();  //LED矩阵测试实现
            tmr1s++;         //每隔1s执行以下分支
            if (tmr1s >= 10)
            {
                tmr1s = 0;
                if (Get18B20Temp(&curTemp))  //读取当前温度
                {
                    ShowTempValue(curTemp);  //刷新温度显示
                }
                Start18B20();   //重新启动下一次转换
            }
        }
    }
}
/* EEPROM检测函数，检查其是否工作正常 */
void CheckE2rom()
{
    uint8 buf[2];
    
    buf[0] = 0x33;  //设置检测数据
    buf[1] = 0xAA;
    E2Write(buf, 0x60, 2);  //在地址0x60写入数据
    buf[0] = 0x00;  //清零检测数据
    buf[1] = 0x00;
    E2Read(buf, 0x60, 2);   //在地址0x60读出数据
    if ((buf[0]==0x33) && (buf[1]==0xAA)) //校验数据
        LedNumber(5, 0xE, 0);  //正常时在数码管最高位显示"E"
    else
        LedNumber(5, 0xE, 1);  //故障时在数码管最高位显示"E."
}
/* FLASH检测函数，检查其是否工作正常 */
void CheckFlash()
{
    uint8 buf[2];
    
    buf[0] = 0x33;  //设置检测数据
    buf[1] = 0xAA;
    FlashSectorErase(0x10000);       //擦除地址0x10000所在扇区
    FlashPageWrite(buf, 0x10000, 2); //在地址0x10000写入数据
    buf[0] = 0x00;  //清零检测数据
    buf[1] = 0x00;
    FlashRead(buf, 0x10000, 2);      //在地址0x10000读出数据
    if ((buf[0]==0x33) && (buf[1]==0xAA)) //校验数据
        LedNumber(4, 0xF, 0);  //正常时在数码管次高位显示"F"
    else
        LedNumber(4, 0xF, 1);  //故障时在数码管次高位显示"F."
}
/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(uint8 keycode)
{
	uint8 str[4];
    
    BuzzBeep();  //蜂鸣器“嘀”
    //"回车"与"Esc"键控制继电器吸合与释放
    if (keycode == 0x0D)
        RELAY = 0;
    else if (keycode == 0x1B)
        RELAY = 1;
	//将键码按十六进制形式转换为字符串
	str[0] = keycode >> 4;
	if (str[0] <= 9)
		str[0] += '0';
	else 
		str[0] += ('A'-10);
	str[1] = keycode & 0xF;
	if (str[1] <= 9)
		str[1] += '0';
	else
		str[1] += ('A'-10);
	str[2] = '\r';
	str[3] = '\n';
	//键码由串口发送
	UartWrite("key:0x", sizeof("key:0x")-1);
	UartWrite(str, 4);
}
/* 串口动作函数，根据接收到的命令帧执行响应的动作
   buf-接收到的命令帧指针，len-命令帧长度 */
void UartAction(uint8 *buf, uint8 len)
{
    UartWrite(buf, len);  //将接收到的数据原样送回
}
/* 蜂鸣器“嘀”一声 */
void BuzzBeep()
{
    tmrBeep = 100; //100毫秒
    BUZZ = 0;
}
/* 配置并启动T0，ms-T0定时时间 */
void ConfigTimer0(uint16 ms)
{
    uint32 tmp;
    
    tmp = (SYS_MCLK*ms)/1000; //计算所需的计数值
    tmp = 65536 - tmp;        //计算定时器重载值
    tmp = tmp + 33;           //补偿中断响应延时造成的误差   
    T0RH = (uint8)(tmp>>8);   //定时器重载值拆分为高低字节
    T0RL = (uint8)tmp;
    TMOD &= 0xF0;   //清零T0的控制位
    TMOD |= 0x01;   //配置T0为模式1
    TH0 = T0RH;     //加载T0重载值
    TL0 = T0RL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
}
/* T0中断服务函数，实现系统定时和按键扫描 */
void InterruptTimer0() interrupt 1
{
    static uint8 tmr100ms = 0;
    
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    LedScan();   //执行LED扫描
    KeyScan();   //执行按键扫描
    UartRxMonitor(1);  //执行串口接收监控
    //100ms定时
    tmr100ms++;
    if (tmr100ms >= 100)
    {
        tmr100ms = 0;
        flag100ms = 1;
    }
    //蜂鸣器定时控制
    if (tmrBeep > 0)
        tmrBeep--;
    else
        BUZZ = 1;
}
