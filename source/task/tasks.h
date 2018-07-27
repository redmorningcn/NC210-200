/*******************************************************************************
* Description  : 任务的头文件,进行函数声明
* Author       : 2018/5/11 星期五, by redmorningcn
*******************************************************************************/

#ifndef  TASKS_H_
#define  TASKS_H_

#include <includes.h>

//osal
void    App_TaskOsalCreate(void);
void    TaskInitTmr(void);
osalEvt TaskTmrEvtProcess(osalTid task_id, osalEvt task_event);

//osal led
void    TaskInitLed(void);
osalEvt TaskLedEvtProcess(osalTid task_id, osalEvt task_event);

//osal display
void    TaskInitDisp(void);
osalEvt TaskDispEvtProcess(osalTid task_id, osalEvt task_event);
void    App_DispDelay( osalTime dly);

//osal store
void    TaskInitStore(void);
osalEvt TaskStoreEvtProcess(osalTid task_id, osalEvt task_event);
u8      BSP_FlashOsInit(void);
void    App_FramPara(void);
void    app_ReadOneRecord(stcFlshRec *pRec,u32 num);

//comm task
void    App_TaskCommCreate(void);
void    App_CommIdle(void);
void    NMB_Tx( MODBUS_CH    *pch,
                CPU_INT08U   *p_reg_tbl,
                CPU_INT16U   nbr_bytes);

void OSSetWdtFlag( OS_FLAGS flag );
void OSRegWdtFlag( OS_FLAGS flag );

#endif

