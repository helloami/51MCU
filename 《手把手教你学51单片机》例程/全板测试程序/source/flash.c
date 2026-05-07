/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套Kingst51单片机开发板 示例源代码
*
*         (c) 版权所有 2025 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：flash.c
* 描  述：Flash芯片W25Qxx系列读写模块
* 版本号：v1.0.0
* 备  注：
*******************************************************************************
*/

#include "flash.h"

/* SPI总线写操作：dat-待写入字节 */
void SPIWrite(uint8 dat)
{
    uint8 mask;  //用于探测字节内某一位值的掩码变量

    for (mask=0x80; mask!=0; mask>>=1) //从高位到低位依次进行
    {
        SPI_SCK = 0;          //拉低SCK
        if ((mask&dat) == 0)  //该位的值输出到MOSI上
            SPI_MOSI = 0;
        else
            SPI_MOSI = 1;
        SPI_SCK = 1;          //再拉高SCK
    }
}
/* SPI总线读操作：返回值-读到的字节 */
uint8 SPIRead()
{
    uint8 mask;
    uint8 dat;

    for (mask=0x80; mask!=0; mask>>=1) //从高位到低位依次进行
    {
        SPI_SCK = 0;       //拉低SCK
        if (SPI_MISO == 0) //读取MISO的值
            dat &= ~mask;  //为0时，dat中对应位清零
        else
            dat |= mask;   //为1时，dat中对应位置1
        SPI_SCK = 1;       //再拉高SCK
    }

    return dat;
}
/* Flash忙等待函数，循环查询busy标志位，至芯片不再忙时函数返回 */
void FlashBusyWait()
{
    uint8 dat;
    
    do {
        SPI_SSEL = 0;
        SPIWrite(FLASH_READ_REG1);  //发送读寄存器1命令
        dat = SPIRead();            //读取寄存器1的值
        SPI_SSEL = 1;
    } while (dat & 0x01);  //判断读回的寄存器最低位，即busy标志位
}
/* Flash读取函数：buf-数据接收指针，addr-起始地址，len-读取长度 */
void FlashRead(uint8 *buf, uint32 addr, uint16 len)
{
    FlashBusyWait();
    //发送读数据命令与地址
    SPI_SSEL = 0;
    SPIWrite(FLASH_READ_DATA);
    SPIWrite(addr>>16);
    SPIWrite(addr>>8);
    SPIWrite(addr);
    //连续读取数据
    while (len--)
    {
        *buf++ = SPIRead();
    }
    SPI_SSEL = 1;
}
/* Flash扇区擦除函数：addr-擦除地址 */
void FlashSectorErase(uint32 addr)
{
    FlashBusyWait();
    //发送写使能命令
    SPI_SSEL = 0;
    SPIWrite(FLASH_WRITE_ENABLE);
    SPI_SSEL = 1;
    //发送擦除命令与地址
    SPI_SSEL = 0;
    SPIWrite(FLASH_SECTOR_ERASE);
    SPIWrite(addr>>16);
    SPIWrite(addr>>8);
    SPIWrite(addr);
    SPI_SSEL = 1;
}
/* Flash页写入函数：
   buf-源数据指针，addr-起始地址，
   len-待写入长度，返回值-实际写入长度；
   本函数不保证将全部数据都写入Flash中，当地址跨页时即停止写入，
   如写入数据需跨页时，请在在调用时自行处理跨页及可能的擦除操作 */
uint16 FlashPageWrite(uint8 *buf, uint32 addr, uint16 len)
{
    uint16 n;
    
    //计算起始地址至下一个页边界的字节数，即可写入的最大字节数
    n = FLASH_PAGE_SIZE - (addr % FLASH_PAGE_SIZE);
    //待写入长度超过可写入最大字节数时，将其重置为最大字节数
    if (len > n)
    {
        len = n;
    }
    FlashBusyWait();
    //发送写使能命令
    SPI_SSEL = 0;
    SPIWrite(FLASH_WRITE_ENABLE);
    SPI_SSEL = 1;
    //发送页写命令及地址
    SPI_SSEL = 0;
    SPIWrite(FLASH_PAGE_WRITE);
    SPIWrite(addr>>16);
    SPIWrite(addr>>8);
    SPIWrite(addr);
    //连续发送待写入数据
    for (n=0; n<len; n++)
    {
        SPIWrite(*buf++);
    }
    SPI_SSEL = 1;
    
    return len;  //返回实际写入的数据长度
}