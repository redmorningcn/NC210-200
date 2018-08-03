/**************************************************************
* Description  : Q560 4G无线模块驱动
* Author       : 2018/6/4 星期一, by redmorningcn
*/

#ifndef	__BSP_Q560_4G_H__
#define	__BSP_Q560_4G_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * INCLUDES
 */
#include <includes.h>

#ifdef   BSP_GLOBALS
#define  BSP_EXT
#else
#define  BSP_EXT  extern
#endif
    
/*******************************************************************************
 * CONSTANTS
 */

    
/*******************************************************************************
 * MACROS
 */
#define     GPS_INFO_GET    ("$GPRMC")                  /* 获取GPS信息 */
#define     GPS_INFO_ASK    ("AT*GPSINFO?")             /* 请求GPS信息 */
#define     GPS_RSSI_ASK    ("AT*CSQ?")                 /* 请求网络信号强度 */
#define     GPS_RSSI_GET    ("+CSQ:")                   /* 请求网络信号强度 */
     
#define     GPS_SCFG_SET    ("AT*GPSCFG=4,5#")          /*  请求gps定位，但只响应查询 */     

     
/*******************************************************************************
 * TYPEDEFS
 */
/***********************************************
* 描述： 2017/12/5,无名沈：将接收缓冲区的值复制到数据缓冲区，给后面的程序使用
参数说明:
命令返回：
[GPS 信息 返回格式]:
*/
__packed
typedef struct __gps_info {
    struct {
        char        HeadInfo[10];               //$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*hh
        char        UtcTime[12];                //<1> UTC 时间， hhmmss.sss(时分秒.毫秒)格式
        char        Av[2];                      //<2> 定位状态， A=有效定位， V=无效定位
        char        Latitude[12];               //<3> 纬度 ddmm.mmmm(度分)格式(前面的 0 也将被传输)
        char        LatitudeNS[2];              //<4> 纬度半球 N(北半球)或 S(南半球)
        char        Longitude[12];              //<5> 经度 dddmm.mmmm(度分)格式(前面的 0 也将被传输)
        char        LongitudeEW[2];             //<6> 经度半球 E(东经)或 W(西经)
        char        Speed[10];                  //<7> 地面速率(000.0~999.9 节，前面的 0 也将被传输)
        char        Course[10];                 //<8> 地面航向(000.0~359.9 度，以正北为参考基准，前面的 0 也将被传输)
        char        UtcDate[8];                 //<9> UTC 日期， ddmmyy(日月年)格式
        char        MagneticDeg[8];             //<10> 磁偏角(000.0~180.0 度，前面的 0 也将被传输)
        char        MagneticDegEW[2];           //<11> 磁偏角方向， E(东)或 W(西)
        char        ModeIndication[2];          //<12> 模式指示(仅 NMEA0183 3.00 版本输出， A=自主定位， D=差分， E=估算， N=数据无效)
    } StrInfo;
    
    struct {
        union{
            struct{
                char    GpsData     :1  ;       //当GPS数据有效时，置位
                char    GpsComm     :1  ;       //和GPS模块有通讯时，置位
                char    recv        :6  ;       //预留位
            };
            char    Flgs;      
        }sta;
        StrLkjTime  Time;                       //<1> UTC 时间， hhmmss.sss(时分秒.毫秒)格式
        char        Av;                         //<2> 定位状态， A=有效定位， V=无效定位
        float       Latitude;                   //<3> 纬度 ddmm.mmmm(度分)格式(前面的 0 也将被传输)
        char        LatitudeNS;                 //<4> 纬度半球 N(北半球)或 S(南半球)
        float       Longitude;                  //<5> 经度 dddmm.mmmm(度分)格式(前面的 0 也将被传输)
        char        LongitudeEW;                //<6> 经度半球 E(东经)或 W(西经)
        u16          Speed;                     //<7> 地面速率(000.0~999.9 节，前面的 0 也将被传输)
        u16          Course;                     //<8> 地面航向(000.0~359.9 度，以正北为参考基准，前面的 0 也将被传输)
    } DecInfo;
} StrGpsInfo;
/*
例如：
有效数据： $GPRMC,225530.000,A,3637.26040,N,11700.56340,E,0.000,97.17,220512,,,D*57
无效数据： $GPRMC,000353.000,V,8960.0000,N,00000.0000,E,0.000,0.00,060180,,,N*4B
*/   

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */
 
/*******************************************************************************
 * LOCAL FUNCTIONS
 */

    
/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************
 * EXTERN FUNCTIONS
 */
u8  Q460ProtocoUnpack(char *pstr,int len);

/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/
#ifdef __cplusplus
}
#endif
#endif