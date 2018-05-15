/*******************************************************************************
* Description  : DS3231驱动
* Author       : 2018/5/15 星期二, by redmorningcn
*******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#include <includes.h>
#include <ds3231.h>
#include <bsp_time.h>
#include <bsp_IIC.h>

     
/*******************************************************************************
 * CONSTANTS
 */
#define DS3231_ADDR     		    0xd0
#define DS3231_SEC      		    0x00
#define DS3231_MIN      		    0x01
#define DS3231_HOUR     		    0x02
#define DS3231_DAY      		    0x03
#define DS3231_DATA     		    0x04
#define DS3231_MONTH    		    0x05
#define DS3231_YEAR     		    0x06

#define DS3231_CONTROL      	    0x0E
#define DS3231_STATUS       	    0x0F
#define DS3231_AGING_OFFSET         0x10
#define DS3231_TEMP_MSB     	    0x11
#define DS3231_TEMP_LSB     	    0x12

/*******************************************************************************
 * TYPEDEFS
 */
 
/*******************************************************************************
 * LOCOAL VARIABLES
 */
/***********************************************
* 描述: OS接口
*/


/*******************************************************************************
 * LOCOAL FUNCTIONS
 */
 
/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */
 
/*******************************************************************************
 * EXTERN FUNCTIONS
 */
 

/*******************************************************************************
* 名    称： bcd2Hex
* 功    能： 将BCD码变为二进制码
* 入口参数： BCD码数据
* 出口参数： 二进行制数
* 作　　者： 无名沈
* 创建日期： 2009-01-08
* 修    改：
* 修改日期：
*******************************************************************************/
BYTE Bcd2Hex(BYTE val)
{
    return ((val&0x0f)+(val >> 4)*10);
}

/*******************************************************************************
* 名    称： hex2Bcd
* 功    能： 
* 入口参数：
* 出口参数： 无
* 作　　者： 无名沈
* 创建日期： 2009-01-08
* 修    改：
* 修改日期：
*******************************************************************************/
BYTE Hex2Bcd(BYTE val)
{
    return ((val / 10) << 4) + val % 10;
}

/*******************************************************************************
 * 名    称： ReadDS3231Byte
 * 功    能： 从DS3231中读出一个字节
 * 入口参数： DS3231设备号， 读取数据的地址
 * 出口参数： 返回读出的值
 * 作    者： 无名沈
 * 创建日期： 2018/1/1
 * 修    改： 
 * 修改日期： 
 * 备    注： 
 *******************************************************************************/
uint8 ReadDS3231Byte(uint8 addr)  
{
    uint8   SlaveAddr;
    uint8   RecData;
    uint8   ack = 1;
    
    SlaveAddr = DS3231_ADDR & (~(0x01));            //从机地址
    
    StartI2C();                                     //启动IIC总线
    
    WriteByteWithI2C(SlaveAddr);                    //写DS3231读取
    
    ack = CheckSlaveAckI2C();                       //读从机应答信号
    if ( ack )
        goto exit;
    
    WriteByteWithI2C(addr);                         //写DS3231读取
    
    ack = CheckSlaveAckI2C();                       //读从机应答信号
    if ( ack )
        goto exit;
    
    StartI2C();                                     //启动IIC总线
    
    SlaveAddr = DS3231_ADDR | 0x01;             	//从机地址
    
    WriteByteWithI2C(SlaveAddr);                    //写DS3231设备地址
    
    ack = CheckSlaveAckI2C();                       //读从机应答信号
    if ( ack )
        goto exit;
    
    RecData = ReadByteWithI2C();                	//接收到的数据
    
    MasterNoAckI2C();                               //主机应答从机
exit:
    StopI2C();                          			//结束IIC总线
    
    return  RecData;
}

/*******************************************************************************
 * 名    称： WriteDS3231Byte
 * 功    能： 写一个字节到DS3231中
 * 入口参数： addr:DS3231地址， Data:要写入的数据
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2018/1/1
 * 修    改： 
 * 修改日期： 
 * 备    注： 
 *******************************************************************************/
void WriteDS3231Byte(uint8 Addr,uint8   Data)  
{
    uint8   SlaveAddr;
    uint8   ack = 1;
    
    StartI2C();                                     //启动IIC总线
    
    SlaveAddr = DS3231_ADDR & (~(0x01));            //从机地址
    
    WriteByteWithI2C(SlaveAddr);                    //写DS3231读取
    
    ack = CheckSlaveAckI2C();                       //读从机应答信号(低为应答)
    if ( ack )
        goto exit;
    
    WriteByteWithI2C(Addr);                         //写DS3231操作地址
    
    ack = CheckSlaveAckI2C();                       //读从机应答信号
    if ( ack )
        goto exit;
    
    WriteByteWithI2C(Data);                         //写DS3231数据
    
    ack = CheckSlaveAckI2C();                       //读从机应答信号
    if ( ack )
        goto exit;
    
exit:
    StopI2C();                          			//结束IIC总线
}      

/*******************************************************************************
* Description  : 读内部温度值，返回值°
* Author       : 2018/5/15 星期二, by redmorningcn
*******************************************************************************/
s8  BSP_RTC_ReadTemp(void)  
{
    s8          TempMsb;

    FRAM_WaitEvent();
    
    TempMsb     = ReadDS3231Byte(DS3231_TEMP_MSB);      //读高位
    
    FRAM_SendEvent();
    
    return  TempMsb;
}

/*******************************************************************************
 * 名    称： ReadTime
 * 功    能： 从DS3231中读出时间
 * 入口参数： 顾虑读出来的时间数据
 * 出口参数： 返回读出的时间
 * 作    者： 无名沈
 * 创建日期： 2018/1/1
 * 修    改： 
 * 修改日期： 
 * 备    注： 
 *******************************************************************************/
uint8 BSP_RTC_ReadTime(stcTime  *sTime)  
{    
    FRAM_WaitEvent();
    
    sTime->Sec      = Bcd2Hex(ReadDS3231Byte(DS3231_SEC));
    sTime->Min      = Bcd2Hex(ReadDS3231Byte(DS3231_MIN));
    sTime->Hour     = Bcd2Hex(ReadDS3231Byte(DS3231_HOUR));
    sTime->Day      = Bcd2Hex(ReadDS3231Byte(DS3231_DATA));
    sTime->Mon      = Bcd2Hex(ReadDS3231Byte(DS3231_MONTH)); 
    sTime->Year     = Bcd2Hex(ReadDS3231Byte(DS3231_YEAR));
    
    FRAM_SendEvent();
    
    return  1;
}

/*******************************************************************************
 * 名    称： WriteTime
 * 功    能： 写DS3231时间
 * 入口参数： 无
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2018/1/1
 * 修    改： 
 * 修改日期： 
 * 备    注： 
 *******************************************************************************/
void BSP_RTC_WriteTime(stcTime  sTime)  
{
    FRAM_WaitEvent();
    
    WriteDS3231Byte(DS3231_SEC,     Hex2Bcd(sTime.Sec));     
    WriteDS3231Byte(DS3231_MIN,     Hex2Bcd(sTime.Min));        
    WriteDS3231Byte(DS3231_HOUR,    Hex2Bcd(sTime.Hour));
    WriteDS3231Byte(DS3231_DATA,    Hex2Bcd(sTime.Day));    
    WriteDS3231Byte(DS3231_MONTH,   Hex2Bcd(sTime.Mon));  
    WriteDS3231Byte(DS3231_YEAR,    Hex2Bcd(sTime.Year));
    
    FRAM_SendEvent();
}

/*******************************************************************************
 * 名    称： BSP_Ds3231Init
 * 功    能： 初始化DS3231设备模式
 * 入口参数： 无
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2018/1/1
 * 修    改： 
 * 修改日期： 
 * 备    注： 
 *******************************************************************************/
void BSP_Ds3231Init(void)  
{
    //总线初始化在在bsp中完成。
    //
    WriteDS3231Byte(DS3231_CONTROL,0); 
    //初始化状态寄存器
    WriteDS3231Byte(DS3231_STATUS,0);
}

/*******************************************************************************
* 名    称： BSP_RX8025T_SetDate()
* 功    能： 初始化RTC设备
* 入口参数：
* 出口参数：
* 作　　者： 无名沈
* 创建日期： 2015-01-26
* 修    改：
* 修改日期：
*******************************************************************************/
BOOL BSP_RTC_Init(void)  
{
    BSP_Ds3231Init();
    tm_now  = TIME_GetCalendarTime(); 
    return TRUE;
}

/*******************************************************************************
* 名    称： BSP_RX8025T_SetDate()
* 功    能： 设置时期：年，月，时，星期，时，分，秒
* 入口参数：
* 出口参数：
* 作　　者： 无名沈
* 创建日期： 2015-01-26
* 修    改：
* 修改日期：
*******************************************************************************/
BOOL BSP_SetTime(struct tm t_tm)
{
    INT08U  time[7];
    INT08U  i;
    
    FRAM_WaitEvent();
    /***********************************************
    * 描述： 判断年是否合法
    */
    if ( t_tm.tm_year > 2000 )
        t_tm.tm_year -= 2000;
    
    /***********************************************
    * 描述： 先读取设置
    */        
    time[0]     = Hex2Bcd(t_tm.tm_sec);
    time[1]     = Hex2Bcd(t_tm.tm_min);
    time[2]     = Hex2Bcd(t_tm.tm_hour);
    time[3]     = Hex2Bcd(t_tm.tm_wday);
    time[4]     = Hex2Bcd(t_tm.tm_mday);
    time[5]     = Hex2Bcd(t_tm.tm_mon);
    time[6]     = Hex2Bcd(t_tm.tm_year);
    
    /***********************************************
    * 描述： 写入时间
    */
    for ( i = 0; i < 7; i++ ) {
        WriteDS3231Byte(i,time[i]);
    }  
    FRAM_SendEvent();
    return TRUE;
}

/*******************************************************************************
* 名    称： BSP_RX8025T_GetTime()
* 功    能： 读取时间：年，月，日，星期，时，分，秒
* 入口参数： *t_tm - 时间接收结构体
* 出口参数：
* 作　　者： 无名沈
* 创建日期： 2015-01-26
* 修    改：
* 修改日期：
*******************************************************************************/
BOOL BSP_GetTime(struct tm *t_tm)
{
    u8 time[7];
    INT08U  i;
    FRAM_WaitEvent();
    
    for ( i = 0; i < 7; i++ ) {
        time[i]    = ReadDS3231Byte(i);
    }
    t_tm->tm_sec     = Bcd2Hex(time[0]);
    t_tm->tm_min     = Bcd2Hex(time[1]);
    t_tm->tm_hour    = Bcd2Hex(time[2]);
    t_tm->tm_wday    =         time[3];
    t_tm->tm_mday    = Bcd2Hex(time[4]);
    t_tm->tm_mon     = Bcd2Hex(time[5]);
    t_tm->tm_year    = Bcd2Hex(time[6]) + 2000;
    
    FRAM_SendEvent();
    
    return TRUE;
}
/*******************************************************************************
 * 				end of file                                                    *
 *******************************************************************************/
