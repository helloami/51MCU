/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套Kingst51单片机开发板 示例源代码
*
*         (c) 版权所有 2025 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第12章 EEPROM单字节读写操作
* 版本号：v1.0.0
* 备  注：详情见第12章12.3.1节
*         用单字节读写模式访问EEPROM，每次+1后写回
*******************************************************************************
*/

#include <reg52.h>


extern void I2CStart();
extern void I2CStop();
extern unsigned char I2CReadNAK();
extern bit I2CWrite(unsigned char dat);
unsigned char E2ReadByte(unsigned char addr);
void E2WriteByte(unsigned char addr, unsigned char dat);

void main()
{
    unsigned char dat;

    dat = E2ReadByte(0x02);    //读取指定地址上的一个字节
    dat++;                     //将其数值+1
    E2WriteByte(0x02, dat);    //再写回到对应的地址上
    
    while (1);
}

/* 读取EEPROM中的一个字节，addr-字节地址 */
unsigned char E2ReadByte(unsigned char addr)
{
    unsigned char dat;
    
    I2CStart();
    I2CWrite(0x50<<1); //寻址器件，后续为写操作
    I2CWrite(addr);    //写入存储地址
    I2CStart();        //发送重复启动信号
    I2CWrite((0x50<<1)|0x01); //寻址器件，后续为读操作
    dat = I2CReadNAK();       //读取一个字节数据
    I2CStop();
    
    return dat;
}
/* 向EEPROM中写入一个字节，addr-字节地址 */
void E2WriteByte(unsigned char addr, unsigned char dat)
{
    I2CStart();
    I2CWrite(0x50<<1); //寻址器件，后续为写操作
    I2CWrite(addr);    //写入存储地址
    I2CWrite(dat);     //写入一个字节数据
    I2CStop();
}
