/*******************************************************************************
* Description  : ͨ��ϵͳ���ļ��ж��Ƶ������Ƶ��˴���������ֲ��
�磺����ϵͳ�еĹ��Ӻ���
* Author       : 2018/5/11 ������, by redmorningcn
*******************************************************************************/
#include <tasks.h>

extern  void    App_ModbusInit (void);
extern  void    App_OS_SetAllHooks (void);

/*******************************************************************************
* Description  : BSP��ʼ�����Ӻ�����������ִ��ǰ����(�����кͲ���ϵͳ�йصĲ���)
* Author       : 2018/5/11 ������, by redmorningcn
*******************************************************************************/
void    BSP_Init_Hook(void)
{
    App_ModbusInit();                   //��ʼ�����ڼ����ڿ�����Ϣ����������Modbus��������

    OS_ERR    err;
    
    /***********************************************
    * �������������Ź���־��
    */
    OSFlagCreate(( OS_FLAG_GRP  *)&Ctrl.Os.WdtEvtFlagGRP,
                 ( CPU_CHAR     *)"Wdt Flag",
                 ( OS_FLAGS      )0,
                 ( OS_ERR       *)&err);
}

extern  void app_init_sctrl(void);
/*******************************************************************************
* Description  : ����ϵͳ���񴴽����Ӻ���
* Author       : 2018/5/11 ������, by redmorningcn
*******************************************************************************/
void OS_TaskCreateHook(void)
{     
    //�豸��ʼ��
    BSP_Ds3231Init();                   //��ʼ��ʱ��оƬ
    BSP_DispInit();                     //��ʼ����ʾ
    //InitFlashIO();
    GPIO_Fram_Init();
    BSP_FlashOsInit(); 
    
    app_init_sctrl();                   //��ʼ��ȫ�ֱ���
    /***********************************************
    * ������ ����UCOS���Ӻ�����
    */
    App_OS_SetAllHooks();
    
    /**************************************************************
    * Description  : ��������
    * Author       : 2018/5/18 ������, by redmorningcn
    */
    App_TaskOsalCreate();               //����osal����     
    
    App_TaskCommCreate();               //��������ͨѶ����
}

/*******************************************************************************
* Description  : �ڴ���OSA����ʱ�����õĹ��Ӻ�����
* Author       : 2018/5/11 ������, by redmorningcn
*******************************************************************************/
void OSAL_TaskCreateHook(void)
{
    osal_add_Task(TaskInitTmr,                      // �����ʼ������ָ��
                  TaskTmrEvtProcess,                // �����¼�������ָ��
                  OS_TASK_PRO_TMR,                  // �������ȼ�
                  OS_TASK_ID_TMR);                  // ����ID
    
    osal_add_Task(TaskInitDisp,                     // �����ʼ������ָ��
                  TaskDispEvtProcess,               // �����¼�������ָ��
                  OS_TASK_PRO_DISP,                 // �������ȼ�
                  OS_TASK_ID_DISP);                 // ����ID 
    
    osal_add_Task(TaskInitStore,                    // �����ʼ������ָ��
                  TaskStoreEvtProcess,              // �����¼�������ָ��
                  OS_TASK_PRO_STORE,                // �������ȼ�
                  OS_TASK_ID_STORE);                // ����ID
    
    osal_add_Task(TaskInitLed,                  // �����ʼ������ָ��
                  TaskLedEvtProcess,                // �����¼�������ָ��
                  OS_TASK_PRO_LED,                  // �������ȼ�
                  OS_TASK_ID_LED);                  // ����ID    
}

