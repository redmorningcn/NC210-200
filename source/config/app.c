/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                        Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : EHS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;


/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskCreate (void);
static  void  AppObjCreate  (void);
static  void  AppTaskStart  (void *p_arg);


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/
int  main (void)    
{
/**************************************************************
* Description  : �����ж�������
* Author       : 2018/6/8 ������, by redmorningcn
*/
    BSP_IntDisAll();                                            /* Disable all interrupts.                              */

#ifdef  APP_RELEASE
    u32 offset = USER_APP_START_ADDR-NVIC_VectTab_FLASH;
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, offset);
#endif
    
    OS_ERR  err;

    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR ) AppTaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_START_PRIO,        
                 (CPU_STK    *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
}

/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;
    
    (void)p_arg;
    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();
    
    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        /* Determine nbr SysTick increments                     */
    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */
    
    Mem_Init();                                                 /* Initialize Memory Management Module                  */
    
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif
    
    CPU_IntDisMeasMaxCurReset();
    
    BSP_WDT_Init(BSP_WDT_MODE_ALL);                             // ��ʼ�����Ź�
    
    AppTaskCreate();                                            /* Create Application Tasks                             */
    
    AppObjCreate();                                             /* Create Application Objects                           */
    
    BSP_LED_Off(0);
    
    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        
        /**************************************************************
        * Description  : ϵͳ������ͨ�����Ź���ʱ����������
        * Author       : 2018/7/26 ������, by redmorningcn
        */
        if(Ctrl.sRunPara.SysSts.SysReset){
            while(1){
                BSP_OS_TimeDlyMs(100); 
            }
        }
        
        /***********************************************
        * ������ ι��
        */
        WdtReset();
        
        /***********************************************
        * ������ �õ�ϵͳ��ǰʱ��
        */
        u32 ticks = OSTimeGet(&err);        
        /***********************************************************************
        * ������ �������Ź���־���飬 �ж��Ƿ�����������ι��
        */
        OSFlagPend(( OS_FLAG_GRP *)&Ctrl.Os.WdtEvtFlagGRP,
                   ( OS_FLAGS     ) Ctrl.Os.WdtEvtFlags,
                   ( OS_TICK      ) 50,
                   ( OS_OPT       ) OS_OPT_PEND_FLAG_SET_ALL,                   //ȫ����һ
                   ( CPU_TS      *) NULL,
                   ( OS_ERR      *)&err);
        
        if(err == OS_ERR_NONE) {                                                //����������ι��
            Ctrl.sRunPara.WdtOutTimes = 120;                                    //��ʱ����������ֵ
            OSFlagPost ((OS_FLAG_GRP *)&Ctrl.Os.WdtEvtFlagGRP,                  //�������б�־
                        (OS_FLAGS     ) Ctrl.Os.WdtEvtFlags,
                        (OS_OPT       ) OS_OPT_POST_FLAG_CLR,
                        (OS_ERR      *) &err);
        } else {                                                                //������������ι��
            if(Ctrl.sRunPara.WdtOutTimes ) {                                                   //ι����ʱ 
                Ctrl.sRunPara.WdtOutTimes--;
                if(Ctrl.sRunPara.WdtOutTimes == 0 )                                            //��ʱ����
                    
                    BSP_LED_Flash( 1, 5, 450, 450);
                /***********************************************
                * ������ 2017/12/1,������ϵͳ����
                */
#if defined     (RELEASE)
                SystemReset();        
#endif  
            } else {
                BSP_LED_Flash( 1, 1, 450, 450);  
            }
        }
        
        /***********************************************
        * ������ ȥ���������е�ʱ�䣬�ȵ�һ������������ʣ����Ҫ��ʱ��ʱ��
        */
        u32 dly   = OS_TICKS_PER_SEC - ( OSTimeGet(&err) - ticks );
        if ( dly  < 1 ) {
            dly = 1;
        } else if ( dly > OS_TICKS_PER_SEC ) {
            dly = OS_TICKS_PER_SEC;
        }
        BSP_OS_TimeDlyMs(dly); 
    }
}

/*
*********************************************************************************************************
*                                      CREATE APPLICATION TASKS
*
* Description:  This function creates the application tasks.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
    OS_TaskCreateHook();
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION EVENTS
*
* Description:  This function creates the application kernel objects.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppObjCreate (void)
{
}

/*******************************************************************************
* ��    �ƣ� OSSetWdtFlag
* ��    �ܣ� ����ι��
* ��ڲ����� ��
* ���ڲ����� ��
* ��    �ߣ� ������
* �������ڣ� 2017/11/18
* ��    �ģ� 
* �޸����ڣ� 
* ��    ע�� 
*******************************************************************************/
void OSRegWdtFlag( OS_FLAGS flag )
{
    /***********************************************
    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
    */
    Ctrl.Os.WdtEvtFlags |= flag;
}

/*******************************************************************************
* ��    �ƣ� OSSetWdtFlag
* ��    �ܣ� ����ι��
* ��ڲ����� ��
* ���ڲ����� ��
* ��    �ߣ� ������
* �������ڣ� 2017/11/18
* ��    �ģ� 
* �޸����ڣ� 
* ��    ע�� 
*******************************************************************************/
void OSSetWdtFlag( OS_FLAGS flag )
{
    OS_ERR    err;
    
    /***********************************************
    * ������ �������Ź���־��λ
    */
    OSFlagPost(( OS_FLAG_GRP  *)&Ctrl.Os.WdtEvtFlagGRP,
                ( OS_FLAGS     ) flag,
                ( OS_OPT       ) OS_OPT_POST_FLAG_SET,
                ( OS_ERR      *) &err);
} 
