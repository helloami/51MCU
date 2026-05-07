/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套Kingst51单片机开发板 示例源代码
*
*         (c) 版权所有 2025 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第12章 I2C寻址示例
* 版本号：v1.0.0
* 备  注：详情见第12章12.2节
*         寻址I2C总线上存在的和不存在的地址
*******************************************************************************
*/

#include <reg52.h>
#include <intrins.h>

#define I2CDelay()  {_nop_();_nop_();_nop_();_nop_();}
sbit I2C_SCL = P3^7;
sbit I2C_SDA = P3^6;

bit I2CAddressing(unsigned char addr);

void main()
{
    I2CAddressing(0x50); //查询地址为0x50的器件
    I2CAddressing(0x62); //查询地址为0x62的器件

    while (1);
}
/* 产生总线起始信号 */
void I2CStart()
{
    I2C_SDA = 1; //首先确保SDA、SCL都是高电平
    I2C_SCL = 1;
    I2CDelay();
    I2C_SDA = 0; //先拉低SDA
    I2CDelay();
    I2C_SCL = 0; //再拉低SCL
}
/* 产生总线停止信号 */
void I2CStop()
{
    I2C_SCL = 0; //首先确保SDA、SCL都是低电平
    I2C_SDA = 0;
    I2CDelay();
    I2C_SCL = 1; //先拉高SCL
    I2CDelay();
    I2C_SDA = 1; //再拉高SDA
    I2CDelay();
}
/* I2C总线写操作，dat-待写入字节，返回值-从机应答位的值 */
bit I2CWrite(unsigned char dat)
{
    bit ack;  //用于暂存应答位的值
    unsigned char mask;  //用于探测字节内某一位值的掩码变量

    for (mask=0x80; mask!=0; mask>>=1) //从高位到低位依次进行
    {
        if ((mask&dat) == 0)  //该位的值输出到SDA上
            I2C_SDA = 0;
        else
            I2C_SDA = 1;
        I2CDelay();
        I2C_SCL = 1;          //拉高SCL
        I2CDelay();
        I2C_SCL = 0;          //再拉低SCL，完成一个位周期
    }
    I2C_SDA = 1;   //8位数据发送完后，主机释放SDA，以检测从机应答
    I2CDelay();
    I2C_SCL = 1;   //拉高SCL
    ack = I2C_SDA; //读取此时的SDA值，即为从机的应答值
    I2CDelay();
    I2C_SCL = 0;   //再拉低SCL完成应答位，并保持住总线

    return ack;    //返回从机应答值
}
/* I2C寻址函数，即检查地址为addr的器件是否存在，返回值-从器件应答值 */
bit I2CAddressing(unsigned char addr)
{
    bit ack;

    I2CStart();  //产生起始位，即启动一次总线操作
    ack = I2CWrite(addr<<1);  //器件地址需左移一位，因寻址命令的最低位
                              //为读写位，用于表示之后的操作是读或写
    I2CStop();   //不需进行后续读写，而直接停止本次总线操作
    
    return ack;
}
