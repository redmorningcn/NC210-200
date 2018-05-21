/*******************************************************************************
* Description  : 串口通讯任务
* Author       : 2018/5/18 星期五, by redmorningcn
*/

/*******************************************************************************
* INCLUDES
*/
#include <includes.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_comm__c = "$Id: $";
#endif

#define APP_TASK_COMM_EN     DEF_ENABLED
#if APP_TASK_COMM_EN == DEF_ENABLED
/*******************************************************************************
* CONSTANTS
*/

/*******************************************************************************
* MACROS
*/
#define CYCLE_TIME_TICKS            (OS_TICKS_PER_SEC * 1)

/*******************************************************************************
* TYPEDEFS
*/

/*******************************************************************************
* LOCAL VARIABLES
*/

/*******************************************************************************
* GLOBAL VARIABLES
*/
/***********************************************
* 描述： 任务控制块（TCB）
*/
static  OS_TCB      AppTaskCommTCB;

/***********************************************
* 描述： 任务堆栈（STACKS）
*/
static  CPU_STK     AppTaskCommStk[ APP_TASK_COMM_STK_SIZE ];

OS_Q                COM_RxQ;

/*******************************************************************************
* LOCAL FUNCTIONS
*/
static  void  AppTaskComm (void *p_arg);
void APP_CommInit(void);

/*******************************************************************************
* GLOBAL FUNCTIONS
*/

/*******************************************************************************
* EXTERN VARIABLES
*/

/*******************************************************************************
* EXTERN FUNCTIONS
*/


/*******************************************************************************/
/**************************************************************
* Description  : App_CommIdle(void)串口的每秒周期处理程序，
* Author       : 2018/5/18 星期五, by redmorningcn
*/
void    App_CommIdle(void)
{
    /**************************************************************
    * Description  : 连接状态判断
    * Author       : 2018/5/18 星期五, by redmorningcn
    */
    if( TaxCom->ConnectTimeOut){
        TaxCom->ConnectTimeOut--;
        if(TaxCom->ConnectTimeOut == 0)         //串口超时
        {
            TaxCom->ConnectFlag   = 0;          //连接失效
        }
    }
    
    if( MtrCom->ConnectTimeOut){
        MtrCom->ConnectTimeOut--;
        if(MtrCom->ConnectTimeOut == 0)         //串口超时
        {
            MtrCom->ConnectFlag   = 0;          //连接失效
        }
    }
    
    if( DtuCom->ConnectTimeOut){
        DtuCom->ConnectTimeOut--;
        if(DtuCom->ConnectTimeOut == 0)         //串口超时
        {
            DtuCom->ConnectFlag   = 0;          //连接失效
            
            /**************************************************************
            * Description  : 超时后，连接状态置默认值
            * Author       : 2018/5/21 星期一, by redmorningcn
            */
            DtuCom->ConnCtrl[0].EnableFlg   = 1;        //允许连接    
            DtuCom->ConnCtrl[0].RecvEndFlg  = 0;        //无数据接收
            DtuCom->ConnCtrl[0].SendFlg     = 0;        //无数发送
            DtuCom->ConnCtrl[0].ConnType    = DATA_COMM;//传输数据  
        }
    }
    
    /**************************************************************
    * Description  : 数据发送判断（DTU）
    有数据没有发送。且串口不在发送状态，且串口在数据通讯状态。
    * Author       : 2018/5/18 星期五, by redmorningcn
    */
    if(     Ctrl.sRecNumMgr.GrsRead < Ctrl.sRecNumMgr.Current
       //&&   DtuCom->ConnCtrl[0].SendFlg     == 0  //(状态不好判断)
       &&   DtuCom->ConnCtrl[0].ConnType    == DATA_COMM
      )       
    {
        OS_ERR      err;
        OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.CommEvtFlagGrp,   //通知DTU，可以进行数据发送
                   ( OS_FLAGS      )COMM_EVT_FLAG_DTU_TX,
                   ( OS_OPT        )OS_OPT_POST_FLAG_SET,
                   ( OS_ERR       *)&err);
    }
}

/**************************************************************
* Description  : commTask任务
* Author       : 2018/5/18 星期五, by redmorningcn
*/
void  App_TaskCommCreate(void)
{
    OS_ERR  err;
    
    /***********************************************
    * 描述： 任务创建
    */
    OSTaskCreate((OS_TCB     *)&AppTaskCommTCB,                     // 任务控制块  （当前文件中定义）
                 (CPU_CHAR   *)"App Task Comm",                     // 任务名称
                 (OS_TASK_PTR ) AppTaskComm,                        // 任务函数指针（当前文件中定义）
                 (void       *) 0,                                  // 任务函数参数
                 (OS_PRIO     ) APP_TASK_COMM_PRIO,                 // 任务优先级，不同任务优先级可以相同，0 < 优先级 < OS_CFG_PRIO_MAX - 2（app_cfg.h中定义）
                 (CPU_STK    *)&AppTaskCommStk[0],                  // 任务栈顶
                 (CPU_STK_SIZE) APP_TASK_COMM_STK_SIZE / 10,        // 任务栈溢出报警值
                 (CPU_STK_SIZE) APP_TASK_COMM_STK_SIZE,             // 任务栈大小（CPU数据宽度 * 8 * size = 4 * 8 * size(字节)）（app_cfg.h中定义）
                 (OS_MSG_QTY  ) 5u,                                 // 可以发送给任务的最大消息队列数量
                 (OS_TICK     ) 0u,                                 // 相同优先级任务的轮循时间（ms），0为默认
                 (void       *) 0,                                  // 是一个指向它被用作一个TCB扩展用户提供的存储器位置
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |               // 允许堆栈检查该任务
                                OS_OPT_TASK_STK_CLR),               // 创建任务时堆栈清零
                 (OS_ERR     *)&err);                               // 指向错误代码的指针，用于创建结果处理
}

/*******************************************************************************
* 名    称： AppTaskComm
* 功    能： 控制任务
* 入口参数： p_arg - 由任务创建函数传入
* 出口参数： 无
* 作    者： wumingshen.
* 创建日期： 2015-02-05
* 修    改：
* 修改日期：
*******************************************************************************/
static  void  AppTaskComm (void *p_arg)
{
    OS_ERR          err;
    OS_TICK         ticks;
    OS_TICK         dly     = CYCLE_TIME_TICKS;
    /***********************************************
    * 描述： 任务初始化
    */
    APP_CommInit();         
    
    /***********************************************
    * 描述： Task body, always written as an infinite loop.
    */
    while (DEF_TRUE) { 

        /***********************************************
        * 描述： 本任务看门狗标志置位
        */
        //OSSetWdtFlag(( OS_FLAGS     ) WDT_FLAG_COMM);
        
        /***********************************************
        * 描述： 等待COMM的标识组
        */
        OS_FLAGS    flags = 
        (OS_FLAGS) OSFlagPend(( OS_FLAG_GRP *)&Ctrl.Os.CommEvtFlagGrp,
                       ( OS_FLAGS     ) Ctrl.Os.CommEvtFlag,
                       ( OS_TICK      ) dly,
                       ( OS_OPT       ) OS_OPT_PEND_FLAG_SET_ANY | 
                                        OS_OPT_PEND_BLOCKING,
                       ( CPU_TS      *) NULL,
                       ( OS_ERR      *)&err);
        
        OS_ERR      terr;
        ticks       = OSTimeGet(&terr);                     // 获取当前OSTick
        
        /***********************************************
        * 描述： 没有错误,有事件发生
        */
        if ( err == OS_ERR_NONE ) {
            OS_FLAGS    flagClr = 0;
            
            /**************************************************************
            * Description  : DTU通讯 接收
            * Author       : 2018/5/18 星期五, by redmorningcn
            */
            if( flags   & COMM_EVT_FLAG_DTU_RX  ) {     //60秒内无通讯，强制启动通讯
                //app_comm_dtu(flags); 
                
                if( flags &      COMM_EVT_FLAG_DTU_RX ) { 
                    flagClr |=  COMM_EVT_FLAG_DTU_RX;   
                }
            }
            
            /**************************************************************
            * Description  : 扩展通讯口（TAX）接收
            * Author       : 2018/5/18 星期五, by redmorningcn
            */
            if(    flags & COMM_EVT_FLAG_TAX_RX ) {
                //app_comm_tax(flags);
                
                if(flags &      COMM_EVT_FLAG_TAX_RX) {      
                    flagClr |=  COMM_EVT_FLAG_TAX_RX;   
                }                
            }
            
            /**************************************************************
            * Description  : DTU发送
            * Author       : 2018/5/18 星期五, by redmorningcn
            */
            if(    flags & COMM_EVT_FLAG_DTU_TX ) {
                //app_comm_tax(flags);
                
                
                if(flags &      COMM_EVT_FLAG_DTU_TX) {      
                    flagClr |=  COMM_EVT_FLAG_DTU_TX;   
                }                
            }
            
            /**************************************************************
            * Description  : MTR发送
            * Author       : 2018/5/18 星期五, by redmorningcn
            */
            if(    flags & COMM_EVT_FLAG_MTR_TX ) {
                DtuCom->ConnectFlag     = 1;                //接收到数据
                DtuCom->ConnectTimeOut  = DTU_TIMEOUT;      //DTU超时计数器重新启动
                //DtuCom->TimeoutEn       = 0;                //关超时标识。发送时启动。
                    
                
                if(flags &      COMM_EVT_FLAG_MTR_TX) {      
                    flagClr |=  COMM_EVT_FLAG_MTR_TX;   
                }                
            }
            
            /**************************************************************
            * Description  : TAX发送
            * Author       : 2018/5/18 星期五, by redmorningcn
            */
            if(    flags & COMM_EVT_FLAG_TAX_TX ) {
                //app_comm_tax(flags);
                
                if(flags &      COMM_EVT_FLAG_TAX_TX) {      
                    flagClr |=  COMM_EVT_FLAG_TAX_TX;   
                }                
            }
            
            /***********************************************
            * 描述： 清除标志
            */
            if ( !flagClr ) {
                flagClr = flags;
            }
            
            /***********************************************
            * 描述： 清除标志位
            */
            OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.CommEvtFlagGrp,
                       ( OS_FLAGS      )flagClr,
                       ( OS_OPT        )OS_OPT_POST_FLAG_CLR,
                       ( OS_ERR       *)&err);
        }
        
        
        /***********************************************
        * 描述： 计算剩余时间
        */
        dly   = CYCLE_TIME_TICKS - ( OSTimeGet(&err) - ticks );
        if ( dly  < 1 ) {
            dly = 1;
        } else if ( dly > CYCLE_TIME_TICKS ) {
            dly = CYCLE_TIME_TICKS;
        }
    }
}

/*******************************************************************************
* 名    称： APP_CommInit
* 功    能： 任务初始化
* 入口参数： 无
* 出口参数： 无
* 作    者： wumingshen.
* 创建日期： 2015-03-28
* 修    改：
* 修改日期：
*******************************************************************************/
void APP_CommInit(void)
{
    OS_ERR err;
    
        /***********************************************
    * 描述： 创建事件标志组,协调comm收发
    */
    OSFlagCreate(( OS_FLAG_GRP  *)&Ctrl.Os.CommEvtFlagGrp,
                 ( CPU_CHAR     *)"App_CommFlag",
                 ( OS_FLAGS      )0,
                 ( OS_ERR       *)&err);
    
    Ctrl.Os.CommEvtFlag =     COMM_EVT_FLAG_MTR_RX              // MTR 接收事件
                            + COMM_EVT_FLAG_DTU_RX              // DTU 接收事件
                            + COMM_EVT_FLAG_TAX_RX              // OTR 接收事件
                            + COMM_EVT_FLAG_MTR_TX              
                            + COMM_EVT_FLAG_DTU_TX              
                            + COMM_EVT_FLAG_TAX_TX              
                            ;
    

    /***********************************************
    * 描述： 初始化MODBUS通信(已在hook.c中处理)
    */        
    //App_ModbusInit();
    
    /***********************************************
    * 描述： 在看门狗标志组注册本任务的看门狗标志
    */
    //OSRegWdtFlag(( OS_FLAGS     ) WDT_FLAG_COMM);
}

/*******************************************************************************
* 				end of file
*******************************************************************************/
#endif
