/*******************************************************************************
* Description  : 铁电FRAM驱动
* Author       : 2018/5/15 星期二, by redmorningcn
*******************************************************************************/

/*******************************************************************************
* INCLUDES
*/
#include <includes.h>
#include <bsp_FRAM.h>
#include <bsp_iic.h>


//-------------------------------------------------------------------------------------------------------
//函数名称: 		WriteFM24CL16()
//功    能:			对FM24CL16指定地址进行写数据
//入口参数:         ADDR:       操作地址    地址：0 ~ sizeof(FM24CL16)
//                  		*DataBuf:   数据缓冲
//                  Len: 	数据长度
//作    者： 	redmorningcn
//创建日期： 	2017-05-15
//出口参数:    	无
//说明：            
//--------------------------------------------------------------------------------------------------------
OPTIMIZE_NONE u8 WriteFM24CL64(u16  Addr, u8 *DataBuf, u32 DataBufLen)            
{
    u32  i = 0;
    u8   SlaveAddr;                                      //从机地址
    u16  AddrTemp = 0;                                   //地址缓存
    
	FRAM_WaitEvent();
    
    AddrTemp = Addr;
    AddrTemp += FRAM_START_ADDR;                            //计算地址
    
    if( (u16)AddrTemp + DataBufLen > FRAM_END_ADDR ) {
        FRAM_SendEvent();
        return  FALSE;                                      //地址未超阶
    }
    
    SlaveAddr = FRAM_DIVICE_ADDR & (~(0x01));               //求FM24CL64设备写地址
    
    StartI2C_2();                                             //启动IIC总线
    
    WriteByteWithI2C_2(SlaveAddr);                            //写FM24CL64地址
    CheckSlaveAckI2C_2();
    
    WriteByteWithI2C_2(Addr >> 8);                            //写FM24CL64高位地址
    CheckSlaveAckI2C_2();
    
    WriteByteWithI2C_2( Addr );                               //写FM24CL64低位地址
    CheckSlaveAckI2C_2();     
    
    for(i = 0; i < DataBufLen; i++) {                       //发送数据
        WriteByteWithI2C_2(DataBuf[i]);                       //写数据
        CheckSlaveAckI2C_2();                                 //读从机应答信号
    }

    StopI2C_2();                                              //停止IIC操作 
    
	FRAM_SendEvent();
    
    return  TRUE;                                           //正确
}

//-------------------------------------------------------------------------------------------------------
//函数名称:         ReadFM24CL16()
//功    能:         读取FM24CL16指定地址进行数据
//入口参数:         ADDR:       操作地址   地址：0 ~ sizeof(FM24CL16)
//                  *DataBuf:   数据缓冲 
//                  Len:        数据长度
//出口参数:         无
//作    者： 	redmorningcn
//创建日期： 	2017-05-15
//说明：            
//--------------------------------------------------------------------------------------------------------
OPTIMIZE_NONE u8 ReadFM24CL64(u16  Addr, u8 *DataBuf, u32 DataBufLen)            
{
    u32  i = 0;
    u8   SlaveAddr;                                      //从机地址
    u16  AddrTemp;                                       //地址缓存
    
	FRAM_WaitEvent();
    
    AddrTemp = Addr;
    
    AddrTemp += FRAM_START_ADDR;                            //计算地址
    
    if( (u16)AddrTemp + DataBufLen > FRAM_END_ADDR ) {
        FRAM_SendEvent();
        return  FALSE;                                      //地址未超阶
    }
    
    SlaveAddr = FRAM_DIVICE_ADDR & (~(0x01));               //求FM24CL64设备写地址
    
    StartI2C_2();                                             //启动IIC总线
    
    WriteByteWithI2C_2(SlaveAddr);                            //写FM24CL64设备写地址
    CheckSlaveAckI2C_2();                                     //读从机应答信号
    
    WriteByteWithI2C_2(Addr >> 8 );                           //写数据高位地址
    CheckSlaveAckI2C_2();                                     //读从机应答信号
    
    WriteByteWithI2C_2( Addr );                               //写数据低位地址
    CheckSlaveAckI2C_2();                                     //读从机应答信号
    
    SlaveAddr = (FRAM_DIVICE_ADDR)|(0x01);                  //求FM24CL64设备读地址
    
    StartI2C_2();                                             //启动IIC总线
    
    WriteByteWithI2C_2(SlaveAddr);                            //写FM24CL16地址
    CheckSlaveAckI2C_2();                                     //读从机应答信号
    
    for(i = 0; i < (DataBufLen -1); i++) {                  //发送数据
   
        DataBuf[i] = ReadByteWithI2C_2();                     //写数据
        MasterAckI2C_2();                                     //主机应答从机
    }
    DataBuf[i] = ReadByteWithI2C_2();                         //写数据
    
    MasterNoAckI2C_2();                                       //主机应答从机

    StopI2C_2();
    
	FRAM_SendEvent();
    
    return  TRUE;                                           //正确
}
