/*******************************************************************************
* Description  : 任务的头文件,进行函数声明
* Author       : 2018/5/11 星期五, by redmorningcn
*******************************************************************************/

#ifndef  TASKS_H_
#define  TASKS_H_

#include <includes.h>


//osal
void  App_TaskOsalCreate(void);
void TaskInitTmr(void);
osalEvt  TaskTmrEvtProcess(osalTid task_id, osalEvt task_event);

//osal led
void TaskInitLed(void);
osalEvt  TaskLedEvtProcess(osalTid task_id, osalEvt task_event);

//osal display
void TaskInitDisp(void);
osalEvt  TaskDispEvtProcess(osalTid task_id, osalEvt task_event);

//osal store
void TaskInitStore(void);
osalEvt  TaskStoreEvtProcess(osalTid task_id, osalEvt task_event);
u8 BSP_FlashOsInit(void);


#endif

