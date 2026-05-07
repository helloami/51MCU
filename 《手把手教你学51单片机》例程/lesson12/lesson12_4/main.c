/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套Kingst51单片机开发板 示例源代码
*
*         (c) 版权所有 2025 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第12章 EEPROM连续读与分页写操作示例
* 版本号：v1.0.0
* 备  注：详情见第12章12.3.3节
*         用连续读与分页写模式访问EEPROM，依次+1,+2,+3...后写回
*******************************************************************************
*/

#include <reg52.h>

extern void E2Read(unsigned char *buf, unsigned char addr, unsigned char len);
extern void E2Write(unsigned char *buf, unsigned char addr, unsigned char len);

void main()
{
    unsigned char i;
    unsigned char buf[5];

    E2Read(buf, 0x8E, sizeof(buf));   //从E2中读取一段数据
    for (i=0; i<sizeof(buf); i++)     //数据依次+1,+2,+3...
    {
        buf[i] = buf[i] + 1 + i;
    }
    E2Write(buf, 0x8E, sizeof(buf));  //再写回到E2中
    
    while(1);
}
