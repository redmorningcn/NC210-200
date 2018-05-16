/*******************************************************************************
* Description  : 数码管显示任务
* Author       : 2018/5/16 星期三, by redmorningcn
*******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#include <includes.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_disp__c = "$Id: $";
#endif

#define APP_TASK_DISP_EN     DEF_ENABLED
#if APP_TASK_DISP_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */
#define CYCLE_TIME_TICKS     (OS_CFG_TICK_RATE_HZ * 1u)

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */
#if ( OSAL_EN == DEF_ENABLED )
#else
/***********************************************
* 描述： 任务控制块（TCB）
*/

/***********************************************
* 描述： 任务堆栈（STACKS）
*/

#endif     
     
static  uint8               dis_mode    = 0;
         uint16              ErrCode     = 0;

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
     
#if ( OSAL_EN == DEF_ENABLED )
#else
static  void                AppTaskDisp           (void *p_arg);
#endif

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */


/*******************************************************************************
 * EXTERN VARIABLES
 */

#define     DIS_INFO_NUM        9
#define     DIS_MAX_ERR_NUM     3      
/*******************************************************************************
* Description  : 装置正常运行时，循环显示的内容。
                 如果有故障，则显示故障代码。
* Author       : 2018/5/16 星期三, by redmorningcn
*******************************************************************************/
void    LkjDisplayInfo(void)
{
    static  u8      times = 0;
    u8              mod = DIS_INFO_NUM + DIS_MAX_ERR_NUM;
    u8              groupstring[]= "ABC";
    u8              groupunm;
    u8              chnum;
    u8              phasenum;
    
    //计算占空比的组及通道号
    if(     times > (DIS_MAX_ERR_NUM-1) 
       &&   times < DIS_MAX_ERR_NUM + 6){
        groupunm = (times - (DIS_MAX_ERR_NUM  ))/2;     //DIS_MAX_ERR_NUM  +1 是由于 times++的
        chnum    = (times - (DIS_MAX_ERR_NUM ))%2;
        
        if(groupunm > 2)        //防越界
            groupunm = 0;
        if(chnum > 1)
            chnum = 0;

    }
    //计算相位差组号
    if(times > (DIS_MAX_ERR_NUM + 6 -1)){
        phasenum = (times - DIS_MAX_ERR_NUM -6);
        
        if(phasenum  > 2)      //防越界
            phasenum = 0;
    }

    switch(times % mod)
    {
        case 0:
            times++;
            //if()      //有故障，显示故障
            //break;
        case 1:
            times++;
            //if()      //有故障，显示故障
            //break;
        case 2:
            times++;
            //if()      //有故障，显示故障
            break;     
        case 3:         //通道1，占空比
        case 4:         //通道2，占空比
        case 5:         //通道3，占空比
        case 6:         //通道4，占空比
        case 7:         //通道5，占空比 
        case 8:         //通道5，占空比 
            times++;
            uprintf("CH%d-%3.1f",(groupunm*2 + chnum +1),Ctrl.Rec.speed[groupunm].ch[chnum].ratio);
            break;  
            
        case 9:         //A组，相位差
        case 10:        //B组，相位差
        case 11:        //C组，相位差
            times++;
            uprintf("CH%c-%3.1f",groupstring[phasenum],Ctrl.Rec.speed[phasenum].phase);
            
            if(times == mod)  //重新循环
                times = 0;
            break;  
            
        default:
            times = 0;
            break;
    }
}

/*******************************************************************************
 * 名    称： TaskDispEvtProcess
 * 功    能： 
 * 入口参数： 无
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2017/12/5
 * 修    改： 
 * 修改日期： 
 * 备    注： 
 *******************************************************************************/
osalEvt  TaskDispEvtProcess(osalTid task_id, osalEvt task_event)
{
    /***********************************************
    * 描述： 本任务看门狗标志置位
    */
    //OSSetWdtFlag(( OS_FLAGS     ) WDT_FLAG_DISP);
    
    /***********************************************
    * 描述： 2017/11/27,无名沈：
    */
    if( task_event & OS_EVT_DISP_TICKS ) {
        OS_TICK         dly;
        OS_TICK         ticks;
        OS_ERR          err;
        
        /***********************************************
        * 描述： 得到系统当前时间
        */
        ticks = OSTimeGet(&err);
        
        /***********************************************
        * 描述： 根据显示模式显示
        */
        switch(dis_mode) {
            /*******************************************************************
            * 描述： 全亮
            */
        case 0:                                            
            dis_mode++;
            uprintf("8.8.8.8.8.8.8.8.");

            break;
            /*******************************************************************
            * 描述： 显示装置时间:时分秒
            */            
        case 1: 
            dis_mode++;

            tm_now  = TIME_GetCalendarTime(); 
            uprintf("%02d-%02d-%02d",
                    tm_now.tm_year-2000,
                    tm_now.tm_mon,
                    tm_now.tm_mday );
            break;
            /*******************************************************************
            * 描述： 显示装置时间:时分秒
            */
        case 2: 
            dis_mode++;
            
            uprintf("%02d-%02d-%02d",
                    tm_now.tm_hour,
                    tm_now.tm_min,
                    tm_now.tm_sec);  
            break;
            /*******************************************************************
            * 描述： 显示软件版本
            *        
            */
        case 3: 
            dis_mode++;
                uprintf("SV_%05d",Ctrl.sProductInfo.SwVer);  
            break;

        default: 
            
            LkjDisplayInfo();       //正常运行时显示内容
            break;
        }
        /***********************************************
        * 描述： 2018/1/30,无名沈：增加亮度调节,亮度值改变，调整亮度。 
        */
        static  int displevel = 10;  
        if(Ctrl.sRunPara.SysSts.DispLevel != displevel)
        {
            BSP_DispSetBrightness(Ctrl.sRunPara.SysSts.DispLevel); 
            displevel = Ctrl.sRunPara.SysSts.DispLevel;
        }
        /***********************************************
        * 描述： 去除任务运行的时间，等到一个控制周期里剩余需要延时的时间
        */
        dly   = CYCLE_TIME_TICKS - ( OSTimeGet(&err) - ticks );
        if ( dly  < 1 ) {
            dly = 1;
        } else if ( dly > CYCLE_TIME_TICKS ) {
            dly = CYCLE_TIME_TICKS;
        } 
        
        osal_start_timerEx( OS_TASK_ID_DISP,
                           OS_EVT_DISP_TICKS,
                           dly);
        
        return ( task_event ^ OS_EVT_DISP_TICKS );
    }
    
    return 0;
}
/*******************************************************************************
 * 名    称： App_DispDelay
 * 功    能： 将显示任务挂起一段时间，用来显示其他内容
 * 入口参数： 无
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2017/12/20
 * 修    改： 
 * 修改日期： 
 * 备    注： 
 *******************************************************************************/
void App_DispDelay( osalTime dly)
{    
    osal_start_timerEx( OS_TASK_ID_DISP,
                       OS_EVT_DISP_TICKS,
                       dly);
}

/*******************************************************************************
 * 名    称： APP_DispInit
 * 功    能： 任务初始化
 * 入口参数： 无
 * 出口参数： 无
 * 作　 　者： wumingshen.
 * 创建日期： 2015-12-08
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void TaskInitDisp(void)
{    
    /***********************************************
    * 描述： 初始化本任务用到的相关硬件
    */
    BSP_DispInit();
    /***********************************************
    * 描述： 2017/12/3,无名沈：关闭显示
    */
    BSP_DispOff();
    /***********************************************
    * 描述： 2018/1/30,无名沈：增加亮度调节默认参数
    */
    if ( Ctrl.sRunPara.SysSts.DispLevel == 0 )
        Ctrl.sRunPara.SysSts.DispLevel = 10;
    /***********************************************
    * 描述： 2017/12/3,无名沈：设置显示亮度
    */
    BSP_DispSetBrightness(Ctrl.sRunPara.SysSts.DispLevel);
    /***********************************************
    * 描述： 2017/12/3,无名沈：清除所有显示
    */
    BSP_DispClrAll();
    /***********************************************
    * 描述： 2017/12/3,无名沈：开机所有数码管显示8
    */
    uprintf("8.8.8.8.8.8.8.8."); 
    BSP_DispEvtProcess(); 
    
    /***********************************************
    * 描述： 在看门狗标志组注册本任务的看门狗标志
    */
    //OSRegWdtFlag(( OS_FLAGS     )WDT_FLAG_DISP );
    
    /*************************************************
    * 描述：启动事件查询
    */
    osal_start_timerEx( OS_TASK_ID_DISP,
                        OS_EVT_DISP_TICKS,
                        OS_TICKS_PER_SEC);
    
}


/*******************************************************************************
 * 				                    end of file                                *
 *******************************************************************************/
#endif