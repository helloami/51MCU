#include <reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

unsigned char code LedChar[] = {  //数码管显示字符转换表
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[6] = {  //数码管显示缓冲区，初值0xFF确保启动时都不亮
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

/* LED初始化函数 */
void InitLed()
{
    P0 = 0xFF;
    ENLED = 0;
    ADDR3 = 1;
    ADDR2 = 1;
    ADDR1 = 1;
    ADDR0 = 1;
}

/* LED动态扫描函数，在定时中断中调用 */
void LedScan()
{
    static unsigned char i = 0;   //动态扫描的索引，定义为局部静态变量
    
    P0 = 0xFF;   //显示消隐
    switch (i)
    {
        case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=LedBuff[0]; break;
        case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=LedBuff[1]; break;
        case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=LedBuff[2]; break;
        case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=LedBuff[3]; break;
        case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=LedBuff[4]; break;
        case 5: ADDR2=1; ADDR1=0; ADDR0=1; i=0; P0=LedBuff[5]; break;
        default: break;
    }
}

/* 数码管上显示一位数字，index-数码管位索引(从右到左对应0～5)，
   num-待显示的数字，point-代表是否显示该位上的小数点 */
void LedNumber(unsigned char index, unsigned char num, unsigned char point)
{
    LedBuff[index] = LedChar[num];  //输入数字转换为数码管字符0～F
    if (point != 0)
    {
        LedBuff[index] &= 0x7F;  //point不为0时点亮当前位的小数点
    }
}
