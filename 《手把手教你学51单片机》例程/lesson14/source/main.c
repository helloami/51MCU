/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套Kingst51单片机开发板 示例源代码
*
*         (c) 版权所有 2025 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第14章 温度控制系统 工程主文件
* 版本号：v1.0.0
* 备  注：详情见第14章14.5.4节
*******************************************************************************
*/

#include "DS18B20.h"
#include "keyboard.h"
#include "eeprom.h"
#include "Led.h"
#include "main.h"

bit flag2s = 0;   //2s定时标志位
bit staBuzz = 0;  //蜂鸣器状态标志
uint8 T0RH = 0;   //T0重载值的高字节
uint8 T0RL = 0;   //T0重载值的低字节
int16 curTemp = 0;    //当前读取的温度值
int16 actTemp = 0;    //继电器动作温度设定值
int16 alarmTemp = 0;  //高温报警温度设定值
enum eStaSystem staSystem = E_NORMAL;  //系统运行状态

void main()
{
    EA = 1;           //开总中断
    InitLed();        //初始化LED模块
    Start18B20();     //启动首次温度转换
    ConfigTimer0(1);  //配置T0定时1ms
    
    //读取继电器动作温度，超出有效范围则设置为默认值
    E2Read((uint8*)&actTemp, ACT_TEMP_ADDR, sizeof(actTemp));
    if ((actTemp < ACT_TEMP_MIN) || (actTemp > ACT_TEMP_MAX))
    {
        actTemp = ACT_TEMP_DEFAULT;
    }
    //读取高温报警温度，超出有效范围则设置为默认值
    E2Read((uint8*)&alarmTemp, ALARM_TEMP_ADDR, sizeof(alarmTemp));
    if ((alarmTemp < ALARM_TEMP_MIN) || (alarmTemp > ALARM_TEMP_MAX))
    {
        alarmTemp = ALARM_TEMP_DEFAULT;
    }
    
    while (!flag2s);  //等待2秒
    ShowSystemSta(staSystem);
    ShowLedImage(0);
    
    while (1)  //进入主循环
    {
        KeyDriver(); //执行按键驱动
        if (flag2s)  //每隔2s执行以下分支
        {
            flag2s = 0;
            if (Get18B20Temp(&curTemp))  //读取当前温度
            {
                if (staSystem == E_NORMAL)
                {
                    ShowTempValue(curTemp);  //刷新温度显示
                    TempControl();           //执行温度控制
                }
            }
            Start18B20();   //重新启动下一次转换
        }
    }
}

/* 温度控制函数 */
void TempControl()
{
    //检测执行执行继电器动作
    if (curTemp > actTemp)
    {   //高于设定温度时继电器吸合
        RELAY = 0;
        ShowLedImage(1);
    }
    else if (curTemp < actTemp)
    {   //低于设定温度时继电器释放
        RELAY = 1;
        ShowLedImage(0);
    }
    //检测执行高温报警
    if (curTemp >= alarmTemp)
        staBuzz = 1;
    else
        staBuzz = 0;
}
/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(uint8 keycode)
{
    if (keycode == 0x26)  //向上键，增加温度设定值
    {
        if (staSystem == E_SET_ACT)
        {
            actTemp += (1<<4);
            if (actTemp > ACT_TEMP_MAX)
                actTemp = ACT_TEMP_MAX;
        }
        else if (staSystem == E_SET_ALARM)
        {
            alarmTemp += (1<<4);
            if (alarmTemp > ALARM_TEMP_MAX)
                alarmTemp = ALARM_TEMP_MAX;
        }
    }
    else if (keycode == 0x28)  //向下键，减小温度设定值
    {
        if (staSystem == E_SET_ACT)
        {
            actTemp -= (1<<4);
            if (actTemp < ACT_TEMP_MIN)
                actTemp = ACT_TEMP_MIN;
        }
        else if (staSystem == E_SET_ALARM)
        {
            alarmTemp -= (1<<4);
            if (alarmTemp < ALARM_TEMP_MIN)
                alarmTemp = ALARM_TEMP_MIN;
        }
    }
    else if (keycode == 0x0D)  //回车键，切换运行/设置状态
    {
        switch (staSystem)
        {
            case E_NORMAL:
                staSystem = E_SET_ACT;
                break;
            case E_SET_ACT:
                E2Write((uint8*)&actTemp, ACT_TEMP_ADDR, sizeof(actTemp));
                staSystem = E_SET_ALARM;
                break;
            case E_SET_ALARM:
                E2Write((uint8*)&alarmTemp, ALARM_TEMP_ADDR, sizeof(alarmTemp));
                staSystem = E_NORMAL;
                TempControl();
                break;
            default:
                break;
        }
        ShowSystemSta(staSystem);
    }
    //根据系统状态刷新温度显示
    switch (staSystem)
    {
        case E_NORMAL: ShowTempValue(curTemp); break;
        case E_SET_ACT: ShowTempValue(actTemp); break;
        case E_SET_ALARM: ShowTempValue(alarmTemp); break;
        default: break;
    }
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
    static uint16 tmr2s = 0;
    static uint16 tmrBuzz = 0;
    
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    LedScan();  //LED扫描显示
    KeyScan();  //矩阵按键扫描
    //定时2s
    tmr2s++;
    if (tmr2s >= 2000)
    {
        tmr2s = 0;
        flag2s = 1;
    }
    //蜂鸣器驱动
    if (staBuzz)  //蜂鸣器启动，实现间隔发声
    {
        if (tmrBuzz < 500) //0~0.5s发声
        {
            BUZZ = 0;
            tmrBuzz++;
        }
        else if (tmrBuzz < 1000) //0.5~1s不发声
        {
            BUZZ = 1;
            tmrBuzz++;
        }
        else  //计时到1s后重新开始
        {
            tmrBuzz = 0;
        }
    }
    else  //蜂鸣器关闭
    {
        BUZZ = 1;
        tmrBuzz = 0;
    }
}
