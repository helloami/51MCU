/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套Kingst51单片机开发板 示例源代码
*
*         (c) 版权所有 2025 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：Led.c
* 描  述：点阵LED、数码管、独立LED驱动模块
* 版本号：v1.0.0
* 备  注：
*******************************************************************************
*/

#include "Led.h"

uint8 code LedChar[] = {  //数码管显示字符转换表
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
uint8 code LedImage[] = {  //点阵图像(爱心)
    0xFF, 0x99, 0x00, 0x00, 0x00, 0x81, 0xC3, 0xE7
};
struct sLedBuff ledBuff; //LED显示缓冲区，默认初值全0，正好达到上电全亮的效果

/* LED初始化函数，初始化IO、配置定时器 */
void InitLed()
{
    P0 = 0xFF;
    ENLED = 0;
    ADDR3 = 1;
    ADDR2 = 1;
    ADDR1 = 1;
    ADDR0 = 1;
}
/* 用独立LED显示系统状态 */
void ShowSystemSta(enum eStaSystem sta)
{
    if (sta == E_SET_ACT)
        ledBuff.single = 0xFE;  //设置继电器动作温度时点亮最低位LED
    else if (sta == E_SET_ALARM)
        ledBuff.single = 0xFD;  //设置高温报警温度时点亮次低位LED
    else
        ledBuff.single = 0xFF;  //其它状态时LED全熄灭
}
/* 数码管上显示一位数字，index-数码管位索引(从右到左对应0～5)，
**     num-待显示的数字，point-代表是否显示此位上的小数点 */
void LedNumber(uint8 index, uint8 num, uint8 point)
{
    ledBuff.number[index] = LedChar[num];  //输入数字转换为数码管字符0～F
    if (point != 0)
    {
        ledBuff.number[index] &= 0x7F;  //point不为0时点亮当前位的小数点
    }
}
/* 在数码管上显示温度值，温度值格式为DS18B20数据格式 */
void ShowTempValue(int16 temp)
{
    uint8 i;
    uint8 sign;        //温度值的符号位
    int16 intT, decT;  //温度值的整数和小数部分
    
    if (temp >= 0) //温度为正数时，记录符号位，直接分离出整数和小数部分
    {
        sign = 0;
        intT = temp >> 4;
        decT = temp & 0xF;
    }
    else  //温度为负数时，记录符号位，温度转为正数再分离出整数和小数部分
    {
        sign = 1;
        intT = (-temp) >> 4;
        decT = (-temp) & 0xF;
    }
    //处理小数部分的显示
    decT = (decT*10) / 16;  //二进制的小数部分转换为1位十进制位
    LedNumber(0, decT, 0);  //显示小数位
    //处理整数部分的显示
    LedNumber(1, intT%10, 1);  //显示整数个位+小数点
    for (i=2; i<6; i++)        //用循环检测更高位数值
    {
        intT /= 10;
        if (intT > 0)          //不为0时，显示该位数值
            LedNumber(i, intT%10, 0);
        else                   //为0时，则跳出循环
            break;
    }
    //处理符号位的显示
    if (sign == 1)
    {
        ledBuff.number[i++] = 0xBF;
    }
    //更高位清空
    for ( ; i<6; i++)
    {
        ledBuff.number[i] = 0xFF;
    }
}
/* 显示点阵图像，show-显示或清除图像 */
void ShowLedImage(uint8 show)
{
    uint8 i;
    
    if (show)
    {
        for (i=0; i<8; i++)
            ledBuff.array[i] = LedImage[i];
    }
    else
    {
        for (i=0; i<8; i++)
            ledBuff.array[i] = 0xFF;
    }
}
/* LED动态扫描函数，在定时中断中调用 */
void LedScan()
{
    static uint8 i = 0;  //LED位选索引

    P0 = 0xFF;                   //关闭所有段选位，显示消隐
    P1 = (P1 & 0xF0) | i;        //位选索引值赋值到P1口低4位
    P0 = *((uint8*)&ledBuff+i);  //缓冲区中索引位置的数据送到P0口
    if (i < (sizeof(ledBuff)-1)) //索引递增循环，遍历整个缓冲区
        i++;
    else
        i = 0;
}
