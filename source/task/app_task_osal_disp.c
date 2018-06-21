/*******************************************************************************
* Description  : �������ʾ����
* Author       : 2018/5/16 ������, by redmorningcn
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
* ������ ������ƿ飨TCB��
*/

/***********************************************
* ������ �����ջ��STACKS��
*/

#endif     
     
static  uint8       dis_mode    =   0;
         uint16      ErrCode     =   0;

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
* Description  : װ����������ʱ��ѭ����ʾ�����ݡ�
                 ����й��ϣ�����ʾ���ϴ��롣
* Author       : 2018/5/16 ������, by redmorningcn
*******************************************************************************/
void    LkjDisplayInfo(void)
{
    static  u8     times = 0;
    u8              mod = DIS_INFO_NUM + DIS_MAX_ERR_NUM;
    u8              groupstring[]= "ABC";
    u8              groupunm;
    u8              chnum;
    u8              phasenum;
    
    //����ռ�ձȵ��鼰ͨ����
    if(     times > (DIS_MAX_ERR_NUM-1) 
       &&   times < DIS_MAX_ERR_NUM + 6){
        groupunm = (times - (DIS_MAX_ERR_NUM  ))/2;     //DIS_MAX_ERR_NUM  +1 ������ times++��
        chnum    = (times - (DIS_MAX_ERR_NUM ))%2;
        
        if(groupunm > 2)        //��Խ��
            groupunm = 0;
        if(chnum > 1)
            chnum = 0;

    }
    //������λ�����
    if(times > (DIS_MAX_ERR_NUM + 6 -1)){
        phasenum = (times - DIS_MAX_ERR_NUM -6);
        
        if(phasenum  > 2)      //��Խ��
            phasenum = 0;
    }

    switch(times % mod)
    {
        case 0:
            times++;
            //if()      //�й��ϣ���ʾ����
            //break;
        case 1:
            times++;
            //if()      //�й��ϣ���ʾ����
            //break;
        case 2:
            times++;
            //if()      //�й��ϣ���ʾ����
            break;     
        case 3:         //ͨ��1��ռ�ձ�
        case 4:         //ͨ��2��ռ�ձ�
        case 5:         //ͨ��3��ռ�ձ�
        case 6:         //ͨ��4��ռ�ձ�
        case 7:         //ͨ��5��ռ�ձ� 
        case 8:         //ͨ��5��ռ�ձ� 
            times++;
            uprintf("CH%d-%3.1f",(groupunm*2 + chnum +1),Ctrl.Rec.speed[groupunm].ch[chnum].ratio);
            break;  
            
        case 9:         //A�飬��λ��
        case 10:        //B�飬��λ��
        case 11:        //C�飬��λ��
            times++;
            uprintf("CH%c-%3.1f",groupstring[phasenum],Ctrl.Rec.speed[phasenum].phase);
            
            if(times == mod)  //����ѭ��
                times = 0;
            break;  
            
        default:
            times = 0;
            break;
    }
}

/*******************************************************************************
 * ��    �ƣ� TaskDispEvtProcess
 * ��    �ܣ� 
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ��    �ߣ� ������
 * �������ڣ� 2017/12/5
 * ��    �ģ� 
 * �޸����ڣ� 
 * ��    ע�� 
 *******************************************************************************/
osalEvt  TaskDispEvtProcess(osalTid task_id, osalEvt task_event)
{
    /***********************************************
    * ������ �������Ź���־��λ
    */
    //OSSetWdtFlag(( OS_FLAGS     ) WDT_FLAG_DISP);
    
    /***********************************************
    * ������ 2017/11/27,������
    */
    if( task_event & OS_EVT_DISP_TICKS ) {
        OS_TICK         dly;
        OS_TICK         ticks;
        OS_ERR          err;
        
        /***********************************************
        * ������ �õ�ϵͳ��ǰʱ��
        */
        ticks = OSTimeGet(&err);
        
        /***********************************************
        * ������ ������ʾģʽ��ʾ
        */
        switch(dis_mode) {
            /*******************************************************************
            * ������ ȫ��
            */
        case 0:                                            
            dis_mode++;
            uprintf("8.8.8.8.8.8.8.8.");

            break;
            /*******************************************************************
            * ������ ��ʾװ��ʱ��:ʱ����
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
            * ������ ��ʾװ��ʱ��:ʱ����
            */
        case 2: 
            dis_mode++;
            
            uprintf("%02d-%02d-%02d",
                    tm_now.tm_hour,
                    tm_now.tm_min,
                    tm_now.tm_sec);  
            break;
            /*******************************************************************
            * ������ ��ʾ����汾
            *        
            */
        case 3: 
            dis_mode++;
                uprintf("SV_%2d.%02d",Ctrl.sProductInfo.SwVer/100,Ctrl.sProductInfo.SwVer%100);  
            break;

        default: 
            
            LkjDisplayInfo();       //��������ʱ��ʾ����
            break;
        }
        /***********************************************
        * ������ 2018/1/30,�������������ȵ���,����ֵ�ı䣬�������ȡ� 
        */
        static  int displevel = 10;  
        if(Ctrl.sRunPara.SysSts.DispLevel != displevel)
        {
            BSP_DispSetBrightness(Ctrl.sRunPara.SysSts.DispLevel); 
            displevel = Ctrl.sRunPara.SysSts.DispLevel;
        }
        /***********************************************
        * ������ ȥ���������е�ʱ�䣬�ȵ�һ������������ʣ����Ҫ��ʱ��ʱ��
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
 * ��    �ƣ� App_DispDelay
 * ��    �ܣ� ����ʾ�������һ��ʱ�䣬������ʾ��������
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ��    �ߣ� ������
 * �������ڣ� 2017/12/20
 * ��    �ģ� 
 * �޸����ڣ� 
 * ��    ע�� 
 *******************************************************************************/
void App_DispDelay( osalTime dly)
{    
    osal_start_timerEx( OS_TASK_ID_DISP,
                        OS_EVT_DISP_TICKS,
                        dly);
}

/*******************************************************************************
 * ��    �ƣ� APP_DispInit
 * ��    �ܣ� �����ʼ��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� wumingshen.
 * �������ڣ� 2015-12-08
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void TaskInitDisp(void)
{    
    /***********************************************
    * ������ ��ʼ���������õ������Ӳ��
    */
    BSP_DispInit();
    /***********************************************
    * ������ 2017/12/3,�����򣺹ر���ʾ
    */
    BSP_DispOff();
    /***********************************************
    * ������ 2018/1/30,�������������ȵ���Ĭ�ϲ���
    */
    if ( Ctrl.sRunPara.SysSts.DispLevel == 0 )
         Ctrl.sRunPara.SysSts.DispLevel = 10;
    /***********************************************
    * ������ 2017/12/3,������������ʾ����
    */
    BSP_DispSetBrightness(Ctrl.sRunPara.SysSts.DispLevel);
    /***********************************************
    * ������ 2017/12/3,���������������ʾ
    */
    BSP_DispClrAll();
    /***********************************************
    * ������ 2017/12/3,�����򣺿��������������ʾ8
    */
    uprintf("8.8.8.8.8.8.8.8."); 
    BSP_DispEvtProcess(); 
    
    /***********************************************
    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
    */
    //OSRegWdtFlag(( OS_FLAGS     )WDT_FLAG_DISP );
    
    /*************************************************
    * �����������¼���ѯ
    */
    osal_start_timerEx( OS_TASK_ID_DISP,
                        OS_EVT_DISP_TICKS,
                        OS_TICKS_PER_SEC);
    
}


/*******************************************************************************
 * 				                    end of file                                *
 *******************************************************************************/
#endif