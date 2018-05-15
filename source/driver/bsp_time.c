
/*******************************************************************************
 *   Filename:       bsp_time.h
 *   Revised:        $Date: 2014-05-27$
 *   Revision:       $
 *	 Writer:		 Wuming Shen.
 *
 *   Description:
 *   本文件实现基于RTC的日期功能，提供年月日的读写。（基于ANSI-C的time.h）
*
* RTC中保存的时间格式，是UNIX时间戳格式的。即一个32bit的TIME_t变量（实为u32）
*
* ANSI-C的标准库中，提供了两种表示时间的数据  型：
* time_t:    UNIX时间戳（从1970-1-1起到某时间经过的秒数）
* typedef unsigned int time_t;
*
* struct tm: Calendar格式（年月日形式）
* tm结构如下：
* struct tm {
*   int tm_sec;   // 秒 seconds after the minute, 0 to 59
*               (0 - 59 allows for the occasional leap second)
*   int tm_min;   // 分 minutes after the hour, 0 to 59
*   int tm_hour;  // 时 hours since midnight, 0 to 23
*   int tm_mday;  // 日 day of the month, 1 to 31
*   int tm_mon;   // 月 months since January, 0 to 11
*   int tm_year;  // 年 years since 1900
*   int tm_wday;  // 星期 days since Sunday, 0 to 6
*   int tm_yday;  // 从元旦起的天数 days since January 1, 0 to 365
*   int tm_isdst; // 夏令时？？Daylight Savings Time flag
*   ...
* }
*  其中wday，yday可以自动产生，软件直接读取
*  mon的取值为0-11
* ***注意***：
* tm_year:在time.h库中定义为1900年起的年份，即2008年应表示为2008-1900=108
*  这种表示方法对用户来说不是十分友好，与现实有较大差异。
*  所以在本文件中，屏蔽了这种差异。
*  即外部调用本文件的函数时，tm结构体类型的日期，tm_year即为2008
*  注意：若要调用系统库time.c中的函数，需要自行将tm_year-=1900
*
* 成员函数说明：
* struct tm TIME_ConvUnixToCalendar(time_t t);
*  输入一个Unix时间戳（TIME_t），返回Calendar格式日期
* time_t TIME_ConvCalendarToUnix(struct tm t);
*  输入一个Calendar格式日期，返回Unix时间戳（TIME_t）
* time_t TIME_GetUnixTime(void);
*  从RTC取当前时间的Unix时间戳值
* struct tm TIME_GetCalendarTime(void);
*  从RTC取当前时间的日历时间
* void TIME_SetUnixTime(time_t);
*  输入UNIX时间戳格式时间，设置为当前RTC时间
* void TIME_SetCalendarTime(struct tm t);
*  输入Calendar格式时间，设置为当前RTC时间
*
* 外部调用实例：
* 定义一个Calendar格式的日期变量：
* struct tm now;
* now.tm_year = 2008;
* now.tm_mon = 11;  //12月
* now.tm_mday = 20;
* now.tm_hour = 20;
* now.tm_min = 12;
* now.tm_sec = 30;
*
* 获取当前日期时间：
* tm_now = TIME_GetCalendarTime();
* 然后可以直接读tm_now.tm_wday获取星期数
*
* 设置时间：
* Step1. tm_now.xxx = xxxxxxxxx;
* Step2. TIME_SetCalendarTime(tm_now);
*
* 计算两个时间的差
* struct tm t1,t2;
* t1_t = TIME_ConvCalendarToUnix(t1);
* t2_t = TIME_ConvCalendarToUnix(t2);
* dt = t1_t - t2_t;
* dt就是两个时间差的秒数
* dt_tm = mktime(dt); //注意dt的年份匹配，ansi库中函数为相对年份，注意超限
* 另可以参考相关资料，调用ansi-c库的格式化输出等功能，ctime，strftime等
*
 *   Notes:
 *					QQ:276193028
 *     				E-mail:shenchangwei945@163.com
 *
 *   All copyrights reserved to Wuming Shen.
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#include <includes.h>
#include <bsp_time.h>
#include <ds3231.h>

/*******************************************************************************
 * CONSTANTS
 */
#define UTC 8
/*******************************************************************************
 * MACROS
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
struct tm   tm_now = {
  0,30,14,3,9,2015,4
};
time_t  t_now,t_last;

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
void            TIME_Set        (u32 t);
unsigned int    xDate2Seconds   (struct tm *t_tm);
void            xSeconds2Date   (unsigned long seconds,struct tm *time );

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
* Function Name  : TIME_ConvUnixToCalendar(time_t t)
* Description    : 转换UNIX时间戳为日历时间
* Input    : u32 t  当前时间的UNIX时间戳
* Output   : None
* Return   : struct tm
*******************************************************************************/
struct tm TIME_ConvUnixToCalendar(time_t t)
{
    struct tm t_tm;
    xSeconds2Date(t,&t_tm );
    
    return t_tm;
}

/*******************************************************************************
* Function Name  : TIME_ConvCalendarToUnix(struct tm t)
* Description    : 写入RTC时钟当前时间
* Input    : struct tm t
* Output   : None
* Return   : time_t
*******************************************************************************/
time_t TIME_ConvCalendarToUnix(struct tm t)
{
  return xDate2Seconds((struct tm *) &t);
}

/*******************************************************************************
* Function Name  : TIME_GetUnixTime()
* Description    : 从RTC取当前时间的Unix时间戳值
* Input    : None
* Output   : None
* Return   : time_t t
*******************************************************************************/
time_t TIME_GetUnixTime(void)
{
    return TIME_ConvCalendarToUnix(TIME_GetCalendarTime());
}

/*******************************************************************************
* Function Name  : TIME_GetCalendarTime()
* Description    : 从RTC取当前时间的日历时间（struct tm）
* Input    : None
* Output   : None
* Return   : time_t t
*******************************************************************************/
struct tm TIME_GetCalendarTime(void)
{
    struct tm t_tm;
    
    if ( BSP_GetTime(&t_tm) ) {
        return t_tm;
    } else {
        return tm_now;
    }
}

/*******************************************************************************
* Function Name  : TIME_SetUnixTime()
* Description    : 将给定的Unix时间戳写入RTC
* Input    : time_t t
* Output   : None
* Return   : None
*******************************************************************************/
void TIME_SetUnixTime(time_t t)
{    
    TIME_SetCalendarTime(TIME_ConvUnixToCalendar(t));
}

/*******************************************************************************
* Function Name  : TIME_SetCalendarTime()
* Description    : 将给定的Calendar格式时间转换成UNIX时间戳写入RTC
* Input    : struct tm t
* Output   : None
* Return   : None
*******************************************************************************/
void TIME_SetCalendarTime(struct tm t_tm)
{
    BSP_SetTime(t_tm);
}

/*******************************************************************************
* Function Name  : BSP_TIME_Init()
* Description    : 
* Input    : struct tm t
* Output   : None
* Return   : None
*******************************************************************************/
BOOL BSP_TIME_Init(void)
{
    BOOL ret = BSP_RTC_Init();
    /***********************************************
    * 描述： 读取RTC时间
    */
    tm_now  = TIME_GetCalendarTime();

    return ret;
}   

/*******************************************************************************
* Description  : 时钟芯片时间
* Author       : 2018/5/15 星期二, by redmorningcn
*******************************************************************************/
typedef struct t_xtime {
  int year; int month;  int day; 
  int hour; int minute;  int second; 
} _xtime ;

#define xMINUTE     (60)            //1分的秒数
#define xHOUR       (60*xMINUTE)    //1小时的秒数
#define xDAY        (24*xHOUR)      //1天的秒数
#define xYEAR       (365*xDAY)      //1年的秒数

/*******************************************************************************
* Function Name : xDate2Seconds()
* Description   : 将localtime（UTC+8北京时间）转为UNIX TIME，以1970年1月1日为起点 
* Input         : struct tm t
* Output        : None
* Return        : None
*******************************************************************************/
unsigned int  xDate2Seconds(struct tm *t_tm)
{
    const unsigned int  month[12]={
    /*01月*/xDAY*(0),
    /*02月*/xDAY*(31),
    /*03月*/xDAY*(31+28),
    /*04月*/xDAY*(31+28+31),
    /*05月*/xDAY*(31+28+31+30),
    /*06月*/xDAY*(31+28+31+30+31),
    /*07月*/xDAY*(31+28+31+30+31+30),
    /*08月*/xDAY*(31+28+31+30+31+30+31),
    /*09月*/xDAY*(31+28+31+30+31+30+31+31),
    /*10月*/xDAY*(31+28+31+30+31+30+31+31+30),
    /*11月*/xDAY*(31+28+31+30+31+30+31+31+30+31),
    /*12月*/xDAY*(31+28+31+30+31+30+31+31+30+31+30)
  };

  unsigned int  seconds = 0;
  unsigned int  year = 0;

  year      = t_tm->tm_year - 1970;                 //不考虑2100年千年虫问题
  seconds   = xYEAR*year + xDAY*((year+1)/4);       //前几年过去的秒数
  seconds  += month[t_tm->tm_mon-1];                //加上今年本月过去的秒数

  if( (t_tm->tm_mon > 2) && (((year+2)%4)==0) )     //2008年为闰年
    seconds += xDAY;                                //闰年加1天秒数

  seconds += xDAY*(t_tm->tm_mday-1);                //加上本天过去的秒数
  seconds += xHOUR*t_tm->tm_hour;                   //加上本小时过去的秒数
  seconds += xMINUTE*t_tm->tm_min;                  //加上本分钟过去的秒数
  seconds += t_tm->tm_sec;                          //加上当前秒数<br>　seconds -= 8 * xHOUR;
  seconds -= (UTC * 3600);
  
  return seconds;
} 

/*******************************************************************************
* Function Name : xSeconds2Date()
* Description   : 将UNIX时间转为UTC+8 即北京时间
*                 UNIX转为UTC 已进行时区转换 北京时间UTC+8
* Input         : struct tm t
* Output        : None
* Return        : None
*******************************************************************************/
void xSeconds2Date(unsigned long seconds,struct tm *time )
{
    const unsigned int month[12]={
    /*01月*/31,
    /*02月*/28,
    /*03月*/31,
    /*04月*/30,
    /*05月*/31,
    /*06月*/30,
    /*07月*/31,
    /*08月*/31,
    /*09月*/30,
    /*10月*/31,
    /*11月*/30,
    /*12月*/31 
    };

    unsigned int days; 
    unsigned short leap_y_count;  

    seconds        += UTC * 3600;               //时区矫正 转为UTC+8 bylzs
    
    time->tm_sec    = seconds % 60;             //获得秒 
    seconds        /= 60; 
    time->tm_min    =  seconds % 60;            //获得分  
    seconds        /= 60;  
    time->tm_hour   = seconds % 24;             //获得时 
    days            = seconds / 24;             //获得总天数
    leap_y_count    = (days + 365) / 1461;      //过去了多少个闰年(4年一闰)  

    if( ((days + 366) % 1461) == 0) {           //闰年的最后1天  
        time->tm_year   = 1970 + (days / 366);  //获得年  
        time->tm_mon    = 12;                   //调整月  
        time->tm_mday   = 31;  
        return;  
    }  

    days -= leap_y_count; 
    time->tm_year = 1970 + (days / 365);        //获得年  
    days %= 365;                                //今年的第几天  

    days = 01 + days;                           //1日开始  

    if( (time->tm_year % 4) == 0 ) { 
        if(days > 60)
            --days;                             //闰年调整  
        else {  
            if(days == 60){  
                time->tm_mon = 2;  
                time->tm_mday = 29;  
                return;  
            }  
        }  
    }  

    for(time->tm_mon = 0;month[time->tm_mon] < days;time->tm_mon++) {  
        days -= month[time->tm_mon]; 
    }  

    ++time->tm_mon;                             //调整月  
    time->tm_mday = days;                       //获得日  
} 

/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/