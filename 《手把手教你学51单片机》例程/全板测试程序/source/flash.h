/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套Kingst51单片机开发板 示例源代码
*
*         (c) 版权所有 2025 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：flash.c
* 描  述：Flash芯片W25Qxx系列读写模块头文件
* 版本号：v1.0.0
* 备  注：
*******************************************************************************
*/
#ifndef _FLASH_H
#define _FLASH_H

#include "config.h"

#define FLASH_READ_REG1      0x05  //读寄存器1命令
#define FLASH_READ_DATA      0x03  //读数据命令
#define FLASH_WRITE_ENABLE   0x06  //写使能命令
#define FLASH_SECTOR_ERASE   0x20  //扇区擦除命令
#define FLASH_PAGE_WRITE     0x02  //页写入命令
#define FLASH_PAGE_SIZE       256  //每页字节数
#define FLASH_SECTOR_SIZE    4096  //每扇区字节数

void FlashRead(uint8 *buf, uint32 addr, uint16 len);
void FlashSectorErase(uint32 addr);
uint16 FlashPageWrite(uint8 *buf, uint32 addr, uint16 len);

#endif