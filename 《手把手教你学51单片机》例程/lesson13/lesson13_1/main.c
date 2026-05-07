/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套Kingst51单片机开发板 示例源代码
*
*         (c) 版权所有 2025 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第13章 温度传感器DS18B20示例
* 版本号：v1.0.0
* 备  注：详情见第13章13.1节
*         控制DS18B20测量温度并将温度值显示到数码管上
*******************************************************************************
*/

#include <reg52.h>

bit flag1s = 0;          //1s定时标志
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

void ConfigTimer0(unsigned int ms);
extern bit Start18B20();
extern bit Get18B20Temp(int *temp);
void InitLed();
void LedScan();
void LedNumber(unsigned char index, unsigned char num, unsigned char point);


void main()
{
    bit res;
    int temp;        //读取到的当前温度值
    int intT, decT;  //温度值的整数和小数部分

    EA = 1;           //开总中断
    InitLed();        //初始化数码管IO
    Start18B20();     //启动DS18B20
    ConfigTimer0(1);  //T0定时1ms
    
    while (1)
    {
        if (flag1s)  //每秒更新一次温度
        {
            flag1s = 0;
            res = Get18B20Temp(&temp);  //读取当前温度
            if (res)                    //读取成功时，刷新当前温度显示
            {
                intT = temp >> 4;       //分离出温度值整数部分
                decT = temp & 0xF;      //分离出温度值小数部分
                decT = (decT*10) / 16;  //二进制的小数部分转换为1位十进制位
                LedNumber(0, decT, 0);        //显示小数位
                LedNumber(1, intT%10, 1);     //显示整数个位+小数点
                LedNumber(2, intT/10%10, 0);  //显示整数十位
            }
            Start18B20();   //重新启动下一次转换
        }
    }
}

/* 配置并启动T0，ms-T0定时时间 */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //临时变量
    
    tmp = 11059200 / 12;      //定时器计数频率
    tmp = (tmp * ms) / 1000;  //计算所需的计数值
    tmp = 65536 - tmp;        //计算定时器重载值
    tmp = tmp + 33;           //补偿中断响应延时造成的误差
    T0RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //清零T0的控制位
    TMOD |= 0x01;   //配置T0为模式1
    TH0 = T0RH;     //加载T0重载值
    TL0 = T0RL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
}

/* T0中断服务函数，完成1秒定时 */
void InterruptTimer0() interrupt 1
{
    static unsigned int tmr1s = 0;
    
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    LedScan();
    tmr1s++;
    if (tmr1s >= 1000)  //定时1s
    {
        tmr1s = 0;
        flag1s = 1;
    }
}
