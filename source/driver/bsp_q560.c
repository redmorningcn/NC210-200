/**************************************************************
* Description  : Q560 4G无线模块驱动
* Author       : 2018/6/4 星期一, by redmorningcn
*/

/*******************************************************************************
* INCLUDES
*/
#include <includes.h>
#include <global.h>
#include <string.h>
#include <stdlib.h> 
#include <bsp_q560.h>

#define BSP_Q560_4G_EN 1
#if     BSP_Q560_4G_EN > 0
/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
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
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************
 * EXTERN FUNCTIONS
 */

//首先需要认识一下GPS的坐标系。GPS坐标系遵循WGS-84标准，在这个标准下，GPS芯片可以
//发出不同的数据包格式。根据其数据帧帧头的不同，GPS数据可以分类为GPGGA、GPGSA、GPGSV、GPRMC等。
//这些帧头标识了后续帧内数据的组成结构。通常情况下，我们所关心的定位数据如经纬度、
//速度、时间等均可以从GPRMC帧中获取得到。
//在次我不讲解具体的帧的格式，在网上可以很容易搜到，也可以总芯片配套的接口文档找到数据帧的格式。
//通过解读GPRMC包，我们可以得到类似于“3040.8639,N,10405.7573,E”的位置数据，
//其字段含义可以在文档中找到。这里我解析的结果是：北纬30°40.8639'，东经104°5.7573'。
//在得到这两个数据后，我们可以试着去在地图上定位一下，我们发现多数地图定位用的是小数形式的坐标。
//这里我们就需要去讲如何把一个GPS的原始数据，转换为小数形式的数据。
//举例：106°14'15"的转换
//因为度分秒都是六十进制的
//所以可以这样转换：
//15/60=0.25分
//(14+0.25)/60=0.2375度
//106+0.2375=106.2375度
//所以最后的结果是106.2375°
//通过这个转换之后，我的位置坐标【北纬30°40.8639'，东经104°5.7573'】就可以变成
//【30.681065N,104.095955E】（这个就是WGS-84的坐标）这个模样。这个坐标是GPS的物理定位，
//根据国际标准，需要对这个坐标进行GCJ-02偏移转换，转换后的坐标才可以在google地图、高德地图、
//以及腾讯地图上定位（以上三家遵循GCJ-02加密）。至于为什么要这样做，是因为加密坐标的原因。
//因为GCJ-02是不可逆的转换。
//这里贴出由准寻WGS-84标准转换为GCJ-02的C++源码：
#include <math.h>  
const double pi = 3.14159265358979324;  
//  
// Krasovsky 1940  
//   
// a = 6378245.0, 1/f = 298.3   
// b = a * (1 - f)  
// ee = (a^2 - b^2) / a^2;   
const double a      = 6378245.0;  
const double ee     = 0.00669342162296594323;  

static BOOL outOfChina(double lat, double lon)   
{   
    if (lon < 72.004 || lon > 137.8347)  
        return TRUE;   
    if (lat < 0.8293 || lat > 55.8271)  
        return TRUE; 
    return FALSE;  
} 

static double transformLat(double x, double y)  
{   
    double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * sqrt(abs((int)x));  
    ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;   
    ret += (20.0 * sin(y * pi) + 40.0 * sin(y / 3.0 * pi)) * 2.0 / 3.0;   
    ret += (160.0 * sin(y / 12.0 * pi) + 320 * sin(y * pi / 30.0)) * 2.0 / 3.0;   
    return ret;  
}   

static double transformLon(double x, double y)  
{   
    double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(abs((int)x));   
    ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;   
    ret += (20.0 * sin(x * pi) + 40.0 * sin(x / 3.0 * pi)) * 2.0 / 3.0;  
    ret += (150.0 * sin(x / 12.0 * pi) + 300.0 * sin(x / 30.0 * pi)) * 2.0 / 3.0;   
    return ret;  
}   
/* 
参数 
wgLat:WGS-84纬度wgLon:WGS-84经度 
返回值： 
mgLat：GCJ-02纬度mgLon：GCJ-02经度 
*/  
void gps_transform( float wgLat, float wgLon, float *mgLat, float *mgLon)   
{   
    if (outOfChina(wgLat, wgLon)) {   
        *mgLat = wgLat;  
        *mgLon = wgLon;  
        return;   
    }   
    double dLat = transformLat(wgLon - 105.0, wgLat - 35.0);   
    double dLon = transformLon(wgLon - 105.0, wgLat - 35.0);   
    double radLat = wgLat / 180.0 * pi; double magic = sin(radLat);  
    magic = 1 - ee * magic * magic; 
    double sqrtMagic = sqrt(magic);  
    dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);  
    dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * pi);  
    *mgLat = wgLat + dLat; 
    *mgLon = wgLon + dLon;  
} 

//经过以上的转换，我们的坐标就可以在遵循GCJ-02标准的地图删定位了，
//这里给出大家一个比较好用的测试定位的网址：http://www.gpsspg.com/maps.htm
//百度地图有些特殊，它在GCJ-02基础上又进行了一次加密，百度把这个加密标准叫做BD-09，
//但是这个加密并没有对外公开。但是万能的网络以及无所不能的网友总会有办法解决这类问题。
//这里我就贴出一个可行的转换办法，经过测试，定位很准确。
    
const double x_pi = 3.14159265358979324 * 3000.0 / 180.0;    
//将 GCJ-02 坐标转换成 BD-09 坐标  
void bd_encrypt(float gg_lat, float gg_lon, float *bd_lat, float *bd_lon)    
{ 
    double x = gg_lon, y = gg_lat;    
    double z = sqrt(x * x + y * y) + 0.00002 * sin(y * x_pi);    
    double theta = atan2(y, x) + 0.000003 * cos(x * x_pi);    
    *bd_lon = z * cos(theta) + 0.0065;    
    *bd_lat = z * sin(theta) + 0.006;    
}    
    
void bd_decrypt(float bd_lat, float bd_lon, float *gg_lat, float *gg_lon)    
{    
    double x = bd_lon - 0.0065, y = bd_lat - 0.006;    
    double z = sqrt(x * x + y * y) - 0.00002 * sin(y * x_pi);    
    double theta = atan2(y, x) - 0.000003 * cos(x * x_pi);    
    *gg_lon = z * cos(theta);    
    *gg_lat = z * sin(theta);    
} 

/***********************************************
参数说明:
命令返回：
[GPS 信息 返回格式]:
$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*hh
<1> UTC 时间， hhmmss.sss(时分秒.毫秒)格式
<2> 定位状态， A=有效定位， V=无效定位
<3> 纬度 ddmm.mmmm(度分)格式(前面的 0 也将被传输)
<4> 纬度半球 N(北半球)或 S(南半球)
<5> 经度 dddmm.mmmm(度分)格式(前面的 0 也将被传输)
<6> 经度半球 E(东经)或 W(西经)
<7> 地面速率(000.0~999.9 节，前面的 0 也将被传输)
<8> 地面航向(000.0~359.9 度，以正北为参考基准，前面的 0 也将被传输)
<9> UTC 日期， ddmmyy(日月年)格式
<10> 磁偏角(000.0~180.0 度，前面的 0 也将被传输)
<11> 磁偏角方向， E(东)或 W(西)
<12> 模式指示(仅 NMEA0183 3.00 版本输出， A=自主定位， D=差分， E=估算， N=数据无效)
例如：
有效数据： $GPRMC,225530.000,A,3637.26040,N,11700.56340,E,0.000,97.17,220512,,,D*57
无效数据： $GPRMC,000353.000,V,8960.0000,N,00000.0000,E,0.000,0.00,060180,,,N*4B
*/ 
void    GPS_infoUnpack(char *pe )  
{
    char    step    = 0;
    char    *pstr   = NULL;

    for ( step = 0; step < 11; step++, pe++ ) {                 //取个字段。
        pstr    = pe;
        pe      = strstr(pstr,",");
        switch ( step ) {
            //时间
        case 0:
            strncpy(Ctrl.Gps.StrInfo.UtcTime, pstr, sizeof(Ctrl.Gps.StrInfo.UtcTime));
            break;
            //状态
        case 1:
            Ctrl.Gps.DecInfo.sta.GpsData = 1;
            strncpy(Ctrl.Gps.StrInfo.Av, pstr, sizeof(Ctrl.Gps.StrInfo.Av));
            //gps数据无效，直接返回
            if ( Ctrl.Gps.StrInfo.Av[0] != 'A' ) {
                Ctrl.Rec.GpsPos                 = 0;
                Ctrl.Gps.DecInfo.sta.GpsData    = 0;
                return;
            }
            break;
            //纬度
        case 2: {
            strncpy(Ctrl.Gps.StrInfo.Latitude, pstr, sizeof(Ctrl.Gps.StrInfo.Latitude));
            //它采集到的经度和纬度格式分别是
            //纬度ddmm.mmmmm（度分）
            //但实际如果想要在百度地图或Google地图中显示,则需要dd.mmmm和ddd.mmmm
            //下面是转换算法:
            //转换后的纬度 = ddmm.mmmm + (mm.mmmm/60)
            float   g       = atof(Ctrl.Gps.StrInfo.Latitude);
            int     d       = (int)g / 100;
            //转换后的纬度 = ddmm.mmmm + (mm.mmmm/60) 
            float   m       = g - d * 100; 
            g                = d + m / 60;
            Ctrl.Gps.DecInfo.Latitude   = g;
        } break;
        //南北半球
        case 3:
            strncpy(Ctrl.Gps.StrInfo.LatitudeNS, pstr, sizeof(Ctrl.Gps.StrInfo.LatitudeNS));
            break;
            //经度
        case 4: {
            strncpy(Ctrl.Gps.StrInfo.Longitude, pstr, sizeof(Ctrl.Gps.StrInfo.Longitude));
            //它采集到的经度和纬度格式分别是
            //经度dddmm.mmmmm（度分）
            //但实际如果想要在百度地图或Google地图中显示,则需要dd.mmmm和ddd.mmmm
            //下面是转换算法:
            //转换后的经度 = dddmm.mmmm + (mm.mmmm/60)
            float   g       = atof(Ctrl.Gps.StrInfo.Longitude);
            int     d       = (int)g / 100;
            float   m       = g - d * 100; 
            g                = d + m / 60;
            
            Ctrl.Gps.DecInfo.Longitude  = g; 
            
            Ctrl.Rec.Latitude   = (u32)(1000000 * Ctrl.Gps.DecInfo.Latitude);
            Ctrl.Rec.Longitude  = (u32)(1000000 * Ctrl.Gps.DecInfo.Longitude);
        } break;
        //东西半球
        case 5:
            strncpy(Ctrl.Gps.StrInfo.LongitudeEW, pstr,sizeof(Ctrl.Gps.StrInfo.LongitudeEW));
            
            //1东北；2东南；3西北；4西南；其他未知
            if ( Ctrl.Gps.StrInfo.LongitudeEW[0] == 'E' ) {
                if ( Ctrl.Gps.StrInfo.LatitudeNS[0] == 'N' ) 
                    Ctrl.Rec.GpsPos   = 1;
                else
                    Ctrl.Rec.GpsPos   = 2;
            } else {
                if ( Ctrl.Gps.StrInfo.LatitudeNS[0] == 'N' ) 
                    Ctrl.Rec.GpsPos   = 3;
                else
                    Ctrl.Rec.GpsPos   = 4;
            }
            
            break;
            //
        case 6:
            strncpy(Ctrl.Gps.StrInfo.Speed, pstr, sizeof(Ctrl.Gps.StrInfo.Speed));
            Ctrl.Gps.DecInfo.Speed = (u16)atof(Ctrl.Gps.StrInfo.Speed);
            Ctrl.Rec.GpsSpeed    = (u16)(Ctrl.Gps.DecInfo.Speed * 1.852);
            break;
        case 7:
            strncpy(Ctrl.Gps.StrInfo.Course, pstr, sizeof(Ctrl.Gps.StrInfo.Course));
            break;
        case 8:
            strncpy(Ctrl.Gps.StrInfo.UtcDate, pstr, sizeof(Ctrl.Gps.StrInfo.UtcDate)); 
            if ( Ctrl.Gps.StrInfo.Av[0] == 'A' ) {
                time_s  t_tm;
                char    *strs    = Ctrl.Gps.StrInfo.UtcTime;
                char    str[4];
                strncpy(str, &strs[0],2);
                t_tm.tm_hour    = atoi(str);
                strncpy(str, &strs[2],2);
                t_tm.tm_min     = atoi(str);
                strncpy(str, &strs[4],2);
                t_tm.tm_sec     = atoi(str);
                strs            = Ctrl.Gps.StrInfo.UtcDate;
                strncpy(str, &strs[0],2);
                t_tm.tm_mday     = atoi(str);
                strncpy(str, &strs[2],2);
                t_tm.tm_mon     = atoi(str);
                strncpy(str, &strs[4],2);
                t_tm.tm_year    = atoi(str) + 2000;
                time_t  t;
                if ( t_tm.tm_hour < 16 ) {
                    t_tm.tm_hour   += 8;
                    t   = TIME_ConvCalendarToUnix(t_tm);
                } else {
                    t   = TIME_ConvCalendarToUnix(t_tm) + 8*3600;
                }
                //如果时间差超过10秒则校准一次
                if ( abs(t - t_now) > 10 ) {
                    TIME_SetUnixTime(t);
                    tm_now  = TIME_GetCalendarTime();
                }
            }
            break;
        case 9:
            strncpy(Ctrl.Gps.StrInfo.MagneticDeg, pstr,sizeof(Ctrl.Gps.StrInfo.MagneticDeg));
            break;
        case 10:
            strncpy(Ctrl.Gps.StrInfo.MagneticDegEW, pstr, sizeof(Ctrl.Gps.StrInfo.MagneticDegEW));
            break;
        default:
            break;
        }
    }
}


/**************************************************************
* Description  : Q460接收协议。
数据解析成功，返回1；否则返回0；
* Author       : 2018/6/4 星期一, by redmorningcn
*/
u8  Q460ProtocoUnpack(char *pstr,int len)
{ 
    char    *pe     = NULL;
    char    *div    = NULL;
    
    if(len < 255)
        pstr[len] = '\0';             //字符串结束
    
    /**************************************************************
    * Description  : GPS定位信息
    * Author       : 2018/6/4 星期一, by redmorningcn
    */
    pe    = NULL;
    pe = strstr((const char *)pstr, (const char *)GPS_INFO_GET);        //确认是查询指令
    
    if (pe ==  pstr) {
        /**************************************************************
        * Description  : 严格判断。只有字满足条件，且字符在开始位置满足。
        （防止数据帧中出现满足特定字符情况）
        * Author       : 2018/6/4 星期一, by redmorningcn
        */
        Ctrl.Gps.DecInfo.sta.GpsComm    = 1;                //接收到GPS信息。
        
        pe += (strlen(GPS_INFO_GET) + 1);                   //跳过头
        GPS_infoUnpack(pe);                                 //取GPS位置信息
        
        return DEF_TRUE; 
    }
    
    /**************************************************************
    * Description  : GPS信号强度
    * Author       : 2018/6/4 星期一, by redmorningcn
    */
    pe    = NULL;
    pe = strstr((const char *)pstr, (const char *)GPS_RSSI_GET);//确认是查询指令
    
    if (pe ==  pstr) {
        /**************************************************************
        * Description  : 严格判断。只有字满足条件，且字符在开始位置满足。
        （防止数据帧中出现满足特定字符情况）
        * Author       : 2018/6/4 星期一, by redmorningcn
        */
        Ctrl.Gps.DecInfo.sta.GpsComm    = 1;                //接收到GPS信息。
        
        pe += (strlen(GPS_RSSI_GET) );                      //跳过头
        div = strstr(pe,"\r\n");
        if ( NULL != div ) {
            *div = '\0';
            Ctrl.Rec.Rssi = (u8)(atoi(pe));
        }        
        return DEF_TRUE; 
    }
    
    return DEF_FALSE;
}
    
 

/*******************************************************************************
* 				                end of file                                    *
*******************************************************************************/
#endif