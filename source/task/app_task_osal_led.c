/*******************************************************************************
* Description  : LEDָʾ������
* Author       : 2018/5/15 ���ڶ�, by redmorningcn
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
//LEDָʾ��λ����
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
* ��    �ƣ� AppTaskLed
* ��    �ܣ� ��������
* ��ڲ����� p_arg - �����񴴽���������
* ���ڲ����� ��
* ��    �ߣ� wumingshen.
* �������ڣ� 2015-02-05
* ��    �ģ�
* �޸����ڣ�
*******************************************************************************/
osalEvt  TaskLedEvtProcess(osalTid task_id, osalEvt task_event)
{
    /***********************************************
    * ������ �������Ź���־��λ
    */
    OSSetWdtFlag(( OS_FLAGS     ) WDT_FLAG_LED);

    if( task_event & OS_EVT_LED_TICKS ) {
        static  uint8   mode = 0;
        
        /***********************************************
        * ������ ������ʾ
        */
        while( mode < 10) {
            
            if(mode==0 || mode == 9)    
                BSP_LED_Off(0);         //ָʾ��ȫ��
            else
            {
                BSP_LED_On(mode);       //���ε���ָʾ��
            }
            mode++;

            osal_start_timerEx( OS_TASK_ID_LED,
                                OS_EVT_LED_TICKS,
                                500);
            return ( task_event ^ OS_EVT_LED_TICKS );
        }
        
        /***********************************************
        * ����������״̬����ָʾ����˸
        */
  

        /**************************************************************
        * Description  : �ٶ�ͨ�����ź�
        * Author       : 2018/8/21 ���ڶ�, by redmorningcn
        */
        if(     Ctrl.Rec.speed[0].freq 
            ||  Ctrl.Rec.speed[1].freq 
            ||  Ctrl.Rec.speed[2].freq ){
                BSP_LED_Toggle(X1_SPEED_LED);
            }else{
                BSP_LED_Off(X1_SPEED_LED);
            }
        
        /**************************************************************
        * Description  : װ�ú����߷���ģ��ͨѶ
        * Author       : 2018/8/21 ���ڶ�, by redmorningcn
        */
        if(DtuCom->ConnectFlag == 1){        //����������
            
            static  u8  dtuledtimes = 0;
            static  u8  dtucomtimes = 0;
            
            if(Ctrl.sRunPara.SysSts.dtucomflg){ //װ������ͨѶ
                
                BSP_LED_Toggle(X2_DTU_LED);
                
                if(dtucomtimes++ > 10){
                    Ctrl.sRunPara.SysSts.dtucomflg = 0;
                    dtucomtimes = 0;
                }
            }else{
                if(dtuledtimes++ % 5 == 0){     //����
                    BSP_LED_Toggle(X2_DTU_LED);
                }
            }
            
        }else{
            BSP_LED_Off(X2_DTU_LED);
        }
        
        /**************************************************************
        * Description  : �����ź�ָʾ
        * Author       : 2018/8/27 ����һ, by redmorningcn
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
            
               
        BSP_LED_Toggle(RUN_LED);                //����ָʾ��    
        
        /***********************************************
        * ������ ��ʱ������
        */
        osal_start_timerEx( OS_TASK_ID_LED,
                            OS_EVT_LED_TICKS,
                            50);
        return ( task_event ^ OS_EVT_LED_TICKS );
    }
    
    return 0;
}

/*******************************************************************************
* ��    �ƣ�APP_LedInit
* ��    �ܣ������ʼ��
* ��ڲ�������
* ���ڲ�������
* ��    �ߣ�redmorningcn.
* �������ڣ�2017-05-15
* ��    �ģ�
* �޸����ڣ�
*******************************************************************************/
void TaskInitLed(void)
{   
    /***********************************************
    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
    */
    OSRegWdtFlag(( OS_FLAGS     )WDT_FLAG_LED );
    
    /*************************************************
    * �����������¼���ѯ
    */   
    osal_start_timerEx( OS_TASK_ID_LED,
                       OS_EVT_LED_TICKS,
                       OS_TICKS_PER_SEC);
}

/*******************************************************************************
* 				                    end of file                                *
*******************************************************************************/
