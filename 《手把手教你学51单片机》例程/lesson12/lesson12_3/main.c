/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套Kingst51单片机开发板 示例源代码
*
*         (c) 版权所有 2025 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第12章 EEPROM多字节读写操作示例
* 版本号：v1.0.0
* 备  注：详情见第12章12.3.2节
*         用多字节读写模式访问EEPROM，依次+1,+2,+3...后写回
*******************************************************************************
*/

#include <reg52.h>

extern void I2CStart();
extern void I2CStop();
extern unsigned char I2CReadACK();
extern unsigned char I2CReadNAK();
extern bit I2CWrite(unsigned char dat);
void E2Read(unsigned char *buf, unsigned char addr, unsigned char len);
void E2Write(unsigned char *buf, unsigned char addr, unsigned char len);

void main()
{
    unsigned char i;
    unsigned char buf[5];

    E2Read(buf, 0x90, sizeof(buf));   //从E2中读取一段数据
    for (i=0; i<sizeof(buf); i++)     //数据依次+1,+2,+3...
    {
        buf[i] = buf[i] + 1 + i;
    }
    E2Write(buf, 0x90, sizeof(buf));  //再写回到E2中
    
    while(1);
}

/* E2读取函数，buf-数据接收指针，addr-E2中的起始地址，len-读取长度 */
void E2Read(unsigned char *buf, unsigned char addr, unsigned char len)
{
    do {                       //用寻址操作查询当前是否可进行读写操作
        I2CStart();
        if (I2CWrite(0x50<<1)) //应答则跳出循环，非应答则进行下一次查询
        {
            break;
        }
        I2CStop();
    } while(1);
    I2CWrite(addr);            //写入起始地址
    I2CStart();                //发送重复启动信号
    I2CWrite((0x50<<1)|0x01);  //寻址器件，后续为读操作
    while (len > 1)            //连续读取len-1个字节
    {
        *buf++ = I2CReadACK(); //最后字节之前为读取操作+应答
        len--;
    }
    *buf = I2CReadNAK();       //最后一个字节为读取操作+非应答
    I2CStop();
}
/* E2写入函数，buf-源数据指针，addr-E2中的起始地址，len-写入长度 */
void E2Write(unsigned char *buf, unsigned char addr, unsigned char len)
{
    while (len--)
    {
        do {                       //用寻址操作查询当前是否可进行读写操作
            I2CStart();
            if (I2CWrite(0x50<<1)) //应答则跳出循环，非应答则进行下一次查询
            {
                break;
            }
            I2CStop();
        } while(1);
        I2CWrite(addr++);  //写入起始地址
        I2CWrite(*buf++);  //写入一个字节数据
        I2CStop();         //结束写操作，以等待写入完成
    }
}
