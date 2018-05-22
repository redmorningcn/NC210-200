/**************************************************************
* Description  : 处理板和检测板数据通讯
* Author       : 2018/5/22 星期二, by redmorningcn
*/
#ifndef  _APP_MTR_COM_H_
#define  _APP_MTR_COM_H_

#include <includes.h>


/**************************************************************
* Description  : 结构体速度检测板定义
* Author       : 2018/5/22 星期二, by redmorningcn
*/
//通道时间参数缓冲区大小
#define CH_TIMEPARA_BUF_SIZE    50
#define CH_VOLTAGE_BUF_SIZE     20

__packed
typedef struct  
{
    uint32  id;             //产品id       0
    uint32  num;            //产品编号     4
    uint8   buf[64];        //预留         8
    uint32  cpu_freq;       //cpu频率      72
    uint32  time;           //系统全局时间(系统时钟(1/72Mhz) *65536)=约1ms   76
}strSpeedSys;              //size = 80
    
/**************************************************************
* Description  : 速度检测
* Author       : 2018/5/22 星期二, by redmorningcn
*/
__packed
typedef struct
{
   /*******************************************************************************
    * Description  : 通道具体指标
    * Author       : 2018/3/14 星期三, by redmorningcn
    *******************************************************************************/
    struct _strsignalchannelpara_ {
        uint32              period;                         //周期，  0.00-2000000.00us （0.5Hz）
        uint32              freq;                           //频率，  0-100000hz              
        uint16              raise;                          //上升沿，0.00-50.00us
        uint16              fail;                           //下降沿，0.00-50.00us
        uint16              ratio;                          //占空比，0.00-100.00%
        uint16              Vol;                            //低电平，0.00-30.00V
        uint16              Voh;                            //高电平，0.00-30.00V
        uint16              status;                         //通道状态
    }para[2];
    
    uint32  ch1_2phase;                                     //相位差，0.00-360.00°
    uint16  vcc_vol;                                        //供电电压
    uint16  stand_vol;                                      //参考电压
}strSpeed;

/*******************************************************************************
* Description  : 通道结构体。其中，通道时间参数辅助运算；
                             通道参数才是通道的具体指标。
* Author       : 2018/3/13 星期二, by redmorningcn
*******************************************************************************/
__packed
typedef struct
{
    /*******************************************************************************
    * Description  : 辅助运算
    * Author       : 2018/3/14 星期三, by redmorningcn
    *******************************************************************************/
    struct  {
        //时间指标
        struct  {
            uint32  low_up_time;                            //10%位置，上升沿，中断时间 
            uint32  low_down_time;                          //10%位置，下降沿，中断时间
            uint32  hig_up_time;                            //90%位置，上升沿，中断时间
            uint32  hig_down_time;                          //90%位置，下降沿，中断时间
            uint16  low_up_cnt;
            uint16  low_down_cnt;
            uint16  hig_up_cnt;
            uint16  hig_down_cnt;
        }time[CH_TIMEPARA_BUF_SIZE];
        
        uint16  p_write;                                    //缓冲区读写控制
        uint16  p_read;
        uint32  pulse_cnt;                                  //脉冲个数，判断信号有无
        
        //信号所处位置，高/低
        union  
        {
            struct __pluse_status__ {                      // 信号状态
                uint32  raise_10: 1;                        // 上升位置——10%
                uint32  raise_90: 1;       	                // 上升位置--90%
                uint32  fall_90 : 1;       	                // 下降位置--10
                uint32  fall_10 : 1;  	                    // 其他：未定义
                uint32  res     : 28;  	                    // 其他：未定义
            }station;
            uint32  pluse_status;
        };        
        
        //电平指标
        struct {
            uint16  ch_low_voltage;                         //信号低电平
            uint16  ch_hig_voltage;                         //信号高电平
            uint16  vcc_hig_voltage;                        //通道供电电平
        }voltage[CH_VOLTAGE_BUF_SIZE];
        uint16  p_wr_vol;                                   //电平写，指针
        uint16  p_rd_vol;                                   //电平读，指针
    }test[2];                                               //通道检测内容
    
    strSpeed        speed;                             //速度测试
 
}strCoupleChannel;

/**************************************************************
* Description  : 校准参数
* Author       : 2018/5/22 星期二, by redmorningcn
*/
typedef struct {
    u32     line;       //修正线性度
    int16   Deviance;   //修正偏差
    int16   tmp;        //预留

}strCalibration;


/**************************************************************
* Description  : 修正参数表
* Author       : 2018/5/22 星期二, by redmorningcn
*/
typedef struct{
    union   {
        struct{
            strCalibration  VccVol;         //电平
            strCalibration  Ch1Vol;         //通道1电压
            strCalibration  Ch2Vol;         //通道2电压
        };
        strCalibration      CaliBuf[20];
    };
}strCaliTable;


/**************************************************************
* Description  : 速度检测板数据结构
* Author       : 2018/5/22 星期二, by redmorningcn
*/
__packed
typedef union _UnnSpeedctrl_ {
    struct{
        strSpeedSys         sys;            //公共参数
        strCoupleChannel    ch;             //产品特有参数
        strCaliTable        cali;           //校准系数表
        MODBUS_CH   	    *pch;           //modbus控制块
    };
    uint16   buf[512];
        
}UnnSpeedctrl;


#endif

