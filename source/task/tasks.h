/*******************************************************************************
* Description  : �����ͷ�ļ�,���к�������
* Author       : 2018/5/11 ������, by redmorningcn
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

//comm task
void    App_TaskCommCreate(void);
void    App_CommIdle(void);
void    NMB_Tx( MODBUS_CH    *pch,
                CPU_INT08U   *p_reg_tbl,
                CPU_INT16U   nbr_bytes);
#endif

