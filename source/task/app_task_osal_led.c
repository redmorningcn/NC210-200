/*******************************************************************************
* Description  : LED指示灯任务
* Author       : 2018/5/15 星期二, by redmorningcn
*******************************************************************************/


/*******************************************************************************
* INCLUDES
*/
#include <includes.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_led__c = "$Id: $";
#endif

/*******************************************************************************
* CONSTANTS
*/

/*******************************************************************************
* MACROS
*/
//LED指示灯位定义
#define     RUN_LED             1
#define     X1_SPEED_LED        2
#define     X2_DTU_LED          3
#define     X3_TAX_LED          4
#define     X4_VOL_LED          5

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
#if ( OSAL_EN == DEF_ENABLED )
#else
static  void    AppTaskLed           (void *p_arg);
#endif

/*******************************************************************************
* GLOBAL FUNCTIONS
*/

/*******************************************************************************
* EXTERN VARIABLES
*/

/*******************************************************************************/

#define LOCO_WORK_VOL_PER       (10)
#define LOCO_WORK_VOL_MIN       (40)
/*******************************************************************************
* 名    称： AppTaskLed
* 功    能： 控制任务
* 入口参数： p_arg - 由任务创建函数传入
* 出口参数： 无
* 作    者： wumingshen.
* 创建日期： 2015-02-05
* 修    改：
* 修改日期：
*******************************************************************************/
osalEvt  TaskLedEvtProcess(osalTid task_id, osalEvt task_event)
{
    /***********************************************
    * 描述： 本任务看门狗标志置位
    */
    OSSetWdtFlag(( OS_FLAGS     ) WDT_FLAG_LED);

    if( task_event & OS_EVT_LED_TICKS ) {
        static  uint8   mode = 0;
        
        /***********************************************
        * 描述： 开机显示
        */
        while( mode < 10) {
            
            if(mode==0 || mode == 9)    
                BSP_LED_Off(0);         //指示灯全灭
            else
            {
                BSP_LED_On(mode);       //依次点亮指示灯
            }
            mode++;

            osal_start_timerEx( OS_TASK_ID_LED,
                                OS_EVT_LED_TICKS,
                                500);
            return ( task_event ^ OS_EVT_LED_TICKS );
        }
        
        /***********************************************
        * 描述：根据状态控制指示灯闪烁
        */
  

        /**************************************************************
        * Description  : 速度通道有信号
        * Author       : 2018/8/21 星期二, by redmorningcn
        */
        if(     Ctrl.Rec.speed[0].freq 
            ||  Ctrl.Rec.speed[1].freq 
            ||  Ctrl.Rec.speed[2].freq ){
                BSP_LED_Toggle(X1_SPEED_LED);
            }else{
                BSP_LED_Off(X1_SPEED_LED);
            }
        
        /**************************************************************
        * Description  : 装置和无线发送模块通讯
        * Author       : 2018/8/21 星期二, by redmorningcn
        */
        if(DtuCom->ConnectFlag == 1){        //有数据连接
            
            static  u8  dtuledtimes = 0;
            static  u8  dtucomtimes = 0;
            
            if(Ctrl.sRunPara.SysSts.dtucomflg){ //装置正在通讯
                
                BSP_LED_Toggle(X2_DTU_LED);
                
                if(dtucomtimes++ > 10){
                    Ctrl.sRunPara.SysSts.dtucomflg = 0;
                    dtucomtimes = 0;
                }
            }else{
                if(dtuledtimes++ % 5 == 0){     //慢闪
                    BSP_LED_Toggle(X2_DTU_LED);
                }
            }
            
        }else{
            BSP_LED_Off(X2_DTU_LED);
        }
        
        /**************************************************************
        * Description  : 工况信号指示
        * Author       : 2018/8/27 星期一, by redmorningcn
        */
        if(     Ctrl.Rec.Vol.lw/LOCO_WORK_VOL_PER > LOCO_WORK_VOL_MIN
           ||   Ctrl.Rec.Vol.qy/LOCO_WORK_VOL_PER > LOCO_WORK_VOL_MIN
           ||   Ctrl.Rec.Vol.zd/LOCO_WORK_VOL_PER > LOCO_WORK_VOL_MIN
           ||   Ctrl.Rec.Vol.xq/LOCO_WORK_VOL_PER > LOCO_WORK_VOL_MIN
           ||   Ctrl.Rec.Vol.xh/LOCO_WORK_VOL_PER > LOCO_WORK_VOL_MIN

          ){
                BSP_LED_Toggle(X4_VOL_LED);

            }else{
                BSP_LED_Off(X4_VOL_LED);
            }
            
               
        BSP_LED_Toggle(RUN_LED);                //运行指示灯    
        
        /***********************************************
        * 描述： 定时器重启
        */
        osal_start_timerEx( OS_TASK_ID_LED,
                            OS_EVT_LED_TICKS,
                            50);
        return ( task_event ^ OS_EVT_LED_TICKS );
    }
    
    return 0;
}

/*******************************************************************************
* 名    称：APP_LedInit
* 功    能：任务初始化
* 入口参数：无
* 出口参数：无
* 作    者：redmorningcn.
* 创建日期：2017-05-15
* 修    改：
* 修改日期：
*******************************************************************************/
void TaskInitLed(void)
{   
    /***********************************************
    * 描述： 在看门狗标志组注册本任务的看门狗标志
    */
    OSRegWdtFlag(( OS_FLAGS     )WDT_FLAG_LED );
    
    /*************************************************
    * 描述：启动事件查询
    */   
    osal_start_timerEx( OS_TASK_ID_LED,
                       OS_EVT_LED_TICKS,
                       OS_TICKS_PER_SEC);
}

/*******************************************************************************
* 				                    end of file                                *
*******************************************************************************/
