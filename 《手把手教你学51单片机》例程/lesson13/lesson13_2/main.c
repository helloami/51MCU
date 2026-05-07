/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套Kingst51单片机开发板 示例源代码
*
*         (c) 版权所有 2025 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第13章 SPI接口Flash芯片读写示例
* 版本号：v1.0.0
* 备  注：详情见第13章13.3节
*******************************************************************************
*/

#include <reg52.h>

extern void FlashRead(unsigned char *buf, unsigned long addr, unsigned int len);
extern void FlashSectorErase(unsigned long addr);
extern unsigned int FlashPageWrite(unsigned char *buf, unsigned long addr, unsigned int len);

void main()
{
    unsigned char buf[2];    //数据读写缓冲区
    unsigned long addr = 0;  //数据读写地址
    
    FlashRead(buf, addr, 2);      //将数据读入缓冲区
    buf[0] += 1;                  //第一个字节+1
    buf[1] += 2;                  //第二个字节+2
    FlashSectorErase(addr);       //擦除读写地址所在扇区
    FlashPageWrite(buf, addr, 2); //写入缓冲区中的数据
    
    while (1);
}
