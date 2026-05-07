/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套Kingst51单片机开发板 示例源代码
*
*         (c) 版权所有 2025 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第11章 基于帧模式的实用串口程序示例
* 版本号：v1.0.0
* 备  注：详情见第11章11.4节
*         基于命令帧模式，通过UART串口控制蜂鸣器开关
*******************************************************************************
*/

#include <reg52.h>

sbit BUZZ  = P1^6;  //蜂鸣器控制引脚

unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

void ConfigTimer0(unsigned int ms);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);


void main()
{
    EA = 1;            //开总中断
    ConfigTimer0(1);   //配置T0定时1ms
    ConfigUART(9600);  //配置波特率为9600
    
    while (1)
    {
        UartDriver();  //调用串口驱动
    }
}
/* 内存比较函数，比较两个指针所指向的内存数据是否相同，
   ptr1-待比较指针1，ptr2-待比较指针2，len-待比较长度
   返回值-两段内存数据完全相同时返回1，不同返回0 */
bit CmpMemory(unsigned char *ptr1, unsigned char *ptr2, unsigned char len)
{
    while (len--)
    {
        if (*ptr1++ != *ptr2++)  //遇到不相等数据时即刻返回0
        {
            return 0;
        }
    }
    return 1;  //比较完全部长度数据都相等则返回1
}
/* 串口动作函数，根据接收到的命令帧执行响应的动作
   buf-接收到的命令帧指针，len-命令帧长度 */
void UartAction(unsigned char *buf, unsigned char len)
{
    if (CmpMemory(buf, "buzz on", sizeof("buzz on")-1))
    {   //开启蜂鸣器
        BUZZ = 0;
    }
    else if (CmpMemory(buf, "buzz off", sizeof("buzz off")-1))
    {   //关闭蜂鸣器
        BUZZ = 1;
    }
    else
    {   //非有效命令时，给上机发送“错误命令”的提示
        UartWrite("bad command.\r\n", sizeof("bad command.\r\n")-1);
        return;
    }
    //有效命令被执行后，在原命令帧之后添加回车换行符后返回给上位机，表示已执行
    buf[len++] = '\r';
    buf[len++] = '\n';
    UartWrite(buf, len);
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
/* T0中断服务函数，执行串口接收监控和蜂鸣器驱动 */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    UartRxMonitor(1);  //串口接收监控
}
