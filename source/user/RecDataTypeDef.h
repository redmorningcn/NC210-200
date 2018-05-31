/*******************************************************************************
* Description  :  定义存储记录的数据结构
* Author       : 2018/5/15 星期二, by redmorningcn
*******************************************************************************/

#ifndef  _RecDataTypeDef_h_
#define  _RecDataTypeDef_h_

#include <includes.h>

/*******************************************************************************
* Description  : 速度通道存储数据结构
* Author       : 2018/5/15 星期二, by redmorningcn
*******************************************************************************/
typedef struct _stcSpeed
{
    
    union{
        struct{
            u16 freq    :3 ;    //00：频率为0；01：通道1频率为0；02：通道2频率为0；03：频率正常；04：频率偏差大。
            u16 rec     :13;    //预留
        };
        u16 flgs;
    }sta;
    u16     phase;
    u16     vcc;
    u16     freq;
    struct _stcCH
    {
        u16 raise;
        u16 fail;
        u16 ratio;    
        u16 hig;
        u16 low;    
    }ch[2];
}sSpeed;


/*******************************************************************************
* Description  : 定义空检的flash存储的数据结构（128字节）
* Author       : 2018/5/15 星期二, by redmorningcn
*******************************************************************************/
__packed
typedef  struct _stcFlshRec                 //定义空检记录数据结构
{
    u8   				CmdTpye		        ;//命令类型       1
    u8   				EvtType             ;//事件类型       1
    u16   				LocoType            ;//机车型号       2
    u16   				LocoNbr             ;//机 车 号       2
    u32   				RecordId            ;//记录流水号     4
    
    struct {                                //记录时间        4
        u32      Sec                 : 6    ;// D05~D00：秒
        u32      Min                 : 6    ;// D11~D06：分
        u32      Hour                : 5    ;// D16~D12：时
        u32      Day                 : 5    ;// D21~D17：日
        u32      Mon                 : 4    ;// D25~D22：月
        u32      Year                : 6    ;// D31~D26：年
    };                                                  // 
    
    sSpeed              speed[3]            ;//速度通道信号   3*24
    
    struct {  
        u16     vcc                         ;//110V电压
        u16     qy                          ;//牵引电压
        u16     xq                          ;//向前电压
        u16     xh                          ;//向后电压
        u16     lw                          ;//零位电压
    }Vol;                                   //工况电压
    
    u32  				Longitude           ;//经度           4
    u32     			Latitude	        ;//纬度           4
    
    u16   				GpsSpeed       :10  ;//GPS速度         2
    u16   				GpsPos         : 3  ;//东西南北半球   2  1东北；2东南；3西北；4西南；其他未知
    u16                 OffHightFlag   : 1  ;//高度调整标志
    u16   				Rsv            : 2  ;//保留           2
    u8                  Rssi                ;//信号强度       1
    u8                  rsvbyte[3]             ;//预留字节
    
    union __u_sys_err {
        struct __s_sys_err {
        u16                  Sen1OpenErr     : 1;     //D0=1：一端传感器模块故障
        u16                  Sen2OpenErr     : 1;     //D1=1：二端传感器模块故障 
        u16                  Mtr1CommErr     : 1;     //D2=1：一端测量模块通信故障
        u16                  Mtr2CommErr     : 1;     //D3=1：二端测量模块通信故障
        u16                  OtrCommErr      : 1;     //D4=1：IC卡模块通讯故障
        u16                  PwrCommErr      : 1;     //D5=1：电量模块通讯故障
        u16                  ExtCommErr      : 1;     //D6=1：扩展通讯模块通讯故障
        u16                  Disp1CommErr    : 1;     //D7=1：一端显示模块通讯故障
        u16                  Disp2CommErr    : 1;     //D8=1：二端显示模块通讯故障
        u16                  TaxCommErr      : 1;     //D9=1：   TAX通讯故障
        u16                  ParaNullErr     : 1;     //D10=1：未设置运行参数
        u16                  BoxNullErr      : 1;     //D11=1：未设置油箱模型
        u16                  DevNullErr      : 1;     //D12=1：未设置模块
        u16                  BoxSetErr       : 1;     //D13=1：油箱模型设置错误
        u16                  FlashErr        : 1;     //D14=1：存储器故障
        u16                  FramErr         : 1;     //D15=1：铁电故障
        u16                  BattLowErr      : 1;     //D16=1：电池电压低
        u16                  TimeErr         : 1;     //D17=1：时间故障
        u16                  OilLowErr       : 1;     //D18=1：油位低故障
        u16                  OilHighErr      : 1;     //D19=1：油位高故障
        u16                  RsvErr          : 12;     //D23~20：预留
        } Flag;
    u8                  Flags[4];
    } Err;

    u16   				CrcCheck            ;//校验   2                 
} stcFlshRec;


#endif
