/*******************************************************************************
* Description  : ����ͨѶ����
* Author       : 2018/5/18 ������, by redmorningcn
*/

/*******************************************************************************
* INCLUDES
*/
#include <includes.h>
#include <app_dtu_send.h>


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
* ������ ������ƿ飨TCB��
*/
static  OS_TCB      AppTaskCommTCB;

/***********************************************
* ������ �����ջ��STACKS��
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
* Description  : App_CommIdle(void)���ڵ�200ms���ڴ������
* Author       : 2018/5/18 ������, by redmorningcn
*/
void    App_CommIdle(void)
{
    OS_ERR      err;

    /**************************************************************
    * Description  : ����״̬�ж�
    * Author       : 2018/5/18 ������, by redmorningcn
    */
    if( TaxCom->ConnectTimeOut){
        TaxCom->ConnectTimeOut--;
        if(TaxCom->ConnectTimeOut == 0)         //���ڳ�ʱ
        {
            TaxCom->ConnectFlag   = 0;          //����ʧЧ
        }
    }
    
    if( MtrCom->ConnectTimeOut){
        MtrCom->ConnectTimeOut--;
        if(MtrCom->ConnectTimeOut == 0)         //���ڳ�ʱ
        {
            MtrCom->ConnectFlag   = 0;          //����ʧЧ
        }
    }
    
    if( DtuCom->ConnectTimeOut){
        DtuCom->ConnectTimeOut--;
        if(DtuCom->ConnectTimeOut == 0)         //���ڳ�ʱ
        {
            DtuCom->ConnectFlag   = 0;          //����ʧЧ
            
            /**************************************************************
            * Description  : ��ʱ������״̬��Ĭ��ֵ
            * Author       : 2018/5/21 ����һ, by redmorningcn
            */
            DtuCom->ConnCtrl.EnableConnFlg  = 1;        //��������    
            DtuCom->ConnCtrl.RecvEndFlg     = 0;        //�����ݽ���
            DtuCom->ConnCtrl.RecordSendFlg  = 0;        //��������
            DtuCom->ConnCtrl.ConnType       = RECORD_SEND_COMM;//��������  
        }
    }
    
    static  u16  recordtime = 0;
    static  u8   mod = 0;
    
    recordtime++;
    /**************************************************************
    * Description  : ���ݷ����жϣ�DTU��
    ������û�з��͡��Ҵ��ڲ��ڷ���״̬���Ҵ���������ͨѶ״̬������ʱ������
    * Author       : 2018/5/18 ������, by redmorningcn
    //2018.07.04   ����в����������ã�ȡ�����ݷ��͡�
    */
    if(     Ctrl.sRecNumMgr.GrsRead < Ctrl.sRecNumMgr.Current
       //&&   DtuCom->ConnCtrl[0].SendFlg     == 0  //(״̬�����ж�)
       &&   DtuCom->ConnCtrl.ConnType       == RECORD_SEND_COMM
       &&   (u16)Ctrl.sRunPara.StoreTime    == recordtime  
       &&   Ctrl.sRunPara.SysSts.SetBitFlg         == 0        //δ���в������û�iap�����ڲ������û�IAP״̬����Ϊ���㣩
      )       
    {
        recordtime = 0;
        /**************************************************************
        * Description  : ��϶ʱ��ѭ���������ݺ�GPS��ѯ
        * Author       : 2018/6/4 ����һ, by redmorningcn
        */
        mod++;
        if(mod %2 == 0)
            DtuCom->ConnCtrl.ConnType = GPS_COMM;               //��ѯgps
                
        OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.CommEvtFlagGrp,   //֪ͨDTU�����Խ������ݷ���
                   ( OS_FLAGS      )COMM_EVT_FLAG_DTU_TX,
                   ( OS_OPT        )OS_OPT_POST_FLAG_SET,
                   ( OS_ERR       *)&err);
    }
    
    if(Ctrl.sRunPara.SetOutTimes){                              //׼�������������״̬
        Ctrl.sRunPara.SetOutTimes--;
        if(Ctrl.sRunPara.SetOutTimes == 0)
            Ctrl.sRunPara.SysSts.SetBitFlg = 0;
    }
    
    static  u8  mtrtimes = 0;
    mtrtimes++;
    /**************************************************************
    * Description  : MTR����ͨѶ
    û�н�����������ʱ����дsys��cali���������Զ��������ݡ�
    * Author       : 2018/5/22 ���ڶ�, by redmorningcn
    */
    MtrCom->ConnCtrl.protocol = MODBUS_PROTOCOL;
    if(
            MtrCom->ConnCtrl.ConnType == MTR_RD_DETECT 
       &&   MtrCom->ConnCtrl.protocol == MODBUS_PROTOCOL
       &&   mtrtimes > 5) 
    {
        static  u8 node = 0;
        
        mtrtimes = 0;
        node++;                     
        node %= 4;
        MtrCom->ConnCtrl.MB_Node = node+1;                      //modbusЭ��Ķ˿ں�
        
        OSFlagPost(( OS_FLAG_GRP  *)&Ctrl.Os.CommEvtFlagGrp,    //֪ͨMTR���������ݷ���
                   ( OS_FLAGS      )COMM_EVT_FLAG_MTR_TX,
                   ( OS_OPT        )OS_OPT_POST_FLAG_SET,
                   ( OS_ERR       *)&err);
    }
}

/**************************************************************
* Description  : commTask����
* Author       : 2018/5/18 ������, by redmorningcn
*/
void  App_TaskCommCreate(void)
{
    OS_ERR  err;
    
    /***********************************************
    * ������ ���񴴽�
    */
    OSTaskCreate((OS_TCB     *)&AppTaskCommTCB,                     // ������ƿ�  ����ǰ�ļ��ж��壩
                 (CPU_CHAR   *)"App Task Comm",                     // ��������
                 (OS_TASK_PTR ) AppTaskComm,                        // ������ָ�루��ǰ�ļ��ж��壩
                 (void       *) 0,                                  // ����������
                 (OS_PRIO     ) APP_TASK_COMM_PRIO,                 // �������ȼ�����ͬ�������ȼ�������ͬ��0 < ���ȼ� < OS_CFG_PRIO_MAX - 2��app_cfg.h�ж��壩
                 (CPU_STK    *)&AppTaskCommStk[0],                  // ����ջ��
                 (CPU_STK_SIZE) APP_TASK_COMM_STK_SIZE / 10,        // ����ջ�������ֵ
                 (CPU_STK_SIZE) APP_TASK_COMM_STK_SIZE,             // ����ջ��С��CPU���ݿ�� * 8 * size = 4 * 8 * size(�ֽ�)����app_cfg.h�ж��壩
                 (OS_MSG_QTY  ) 5u,                                 // ���Է��͸�����������Ϣ��������
                 (OS_TICK     ) 0u,                                 // ��ͬ���ȼ��������ѭʱ�䣨ms����0ΪĬ��
                 (void       *) 0,                                  // ��һ��ָ����������һ��TCB��չ�û��ṩ�Ĵ洢��λ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |               // �����ջ��������
                                OS_OPT_TASK_STK_CLR),               // ��������ʱ��ջ����
                 (OS_ERR     *)&err);                               // ָ���������ָ�룬���ڴ����������
}

void NMB_Tx(MODBUS_CH    *pch,
            CPU_INT08U   *p_reg_tbl,
            CPU_INT16U   nbr_bytes);
/*******************************************************************************
* ��    �ƣ� AppTaskComm
* ��    �ܣ� ��������
* ��ڲ����� p_arg - �����񴴽���������
* ���ڲ����� ��
* ��    �ߣ� wumingshen.
* �������ڣ� 2015-02-05
* ��    �ģ�
* �޸����ڣ�
*******************************************************************************/
static  void  AppTaskComm (void *p_arg)
{
    OS_ERR          err;
    OS_TICK         ticks;
    OS_TICK         dly     = CYCLE_TIME_TICKS;
    static         u8   times = 0;
    /***********************************************
    * ������ �����ʼ��
    */
    APP_CommInit();         
    
    /***********************************************
    * ������ Task body, always written as an infinite loop.
    */
    while (DEF_TRUE) { 

        /***********************************************
        * ������ �������Ź���־��λ
        */
        //OSSetWdtFlag(( OS_FLAGS     ) WDT_FLAG_COMM);
        
        /***********************************************
        * ������ �ȴ�COMM�ı�ʶ��
        */
        OS_FLAGS    flags = 
        (OS_FLAGS) OSFlagPend(( OS_FLAG_GRP *)&Ctrl.Os.CommEvtFlagGrp,
                       ( OS_FLAGS     ) Ctrl.Os.CommEvtFlag,
                       ( OS_TICK      ) dly,
                       ( OS_OPT       ) OS_OPT_PEND_FLAG_SET_ANY | 
                                        OS_OPT_PEND_BLOCKING ,
                       ( CPU_TS      *) NULL,
                       ( OS_ERR      *)&err);
        
        OS_ERR      terr;
        ticks       = OSTimeGet(&terr);                     // ��ȡ��ǰOSTick
        
        /***********************************************
        * ������ û�д���,���¼�����
        */
        if ( err == OS_ERR_NONE ) {
            OS_FLAGS    flagClr = 0;
           
            /**************************************************************
            * Description  : MTR�շ�
            * Author       : 2018/5/18 ������, by redmorningcn
            */
            if(    flags & COMM_EVT_FLAG_MTR_RX ) {

                
                if(flags &      COMM_EVT_FLAG_MTR_RX) {      
                    flagClr |=  COMM_EVT_FLAG_MTR_RX;   
                }                
            }
            if(    flags & COMM_EVT_FLAG_MTR_TX ) {
                
                extern void app_mtr_com(void);
                app_mtr_com();          //�����ͼ���ͨѶ����     
                
                if(flags &      COMM_EVT_FLAG_MTR_TX) {      
                    flagClr |=  COMM_EVT_FLAG_MTR_TX;   
                }                
            }            
    
            /**************************************************************
            * Description  : DTUͨѶ �շ�
            * Author       : 2018/5/18 ������, by redmorningcn
            */
            if( flags   & COMM_EVT_FLAG_DTU_RX  ) {    
                //app_comm_dtu(flags); 
                
                app_dtu_rec(DtuCom);                         //��DTUģ�飬���ղ��֡�
                DtuCom->ConnCtrl.Connflg = 1;               //���ӳɹ�
                
                if( flags &      COMM_EVT_FLAG_DTU_RX ) { 
                    flagClr |=   COMM_EVT_FLAG_DTU_RX;   
                }
            }
            if(    flags & COMM_EVT_FLAG_DTU_TX ) {
                
                app_dtu_send(DtuCom);                     //��DTUģ�飬���Ͳ���
                
                if(flags &      COMM_EVT_FLAG_DTU_TX) {      
                    flagClr |=  COMM_EVT_FLAG_DTU_TX;   
                }                
            }

            /**************************************************************
            * Description  : ��չͨѶ�ڣ�TAX���շ�
            * Author       : 2018/5/18 ������, by redmorningcn
            */
            if(    flags & COMM_EVT_FLAG_TAX_RX ) {
                //app_comm_tax(flags);
                //��������
   
                app_dtu_rec(TaxCom);                         //��DTUģ�飬���ղ��֡�
                TaxCom->ConnCtrl.Connflg = 1;               //���ӳɹ�
                                if(flags &      COMM_EVT_FLAG_TAX_RX) {      
                    flagClr |=  COMM_EVT_FLAG_TAX_RX;   
                }                
            }
            if(    flags & COMM_EVT_FLAG_TAX_TX ) {
                //app_comm_tax(flags);
                app_dtu_send(TaxCom);                     //��DTUģ�飬���Ͳ���
 
                
                if(flags &      COMM_EVT_FLAG_TAX_TX) {      
                    flagClr |=  COMM_EVT_FLAG_TAX_TX;   
                }                
            }
 
            /***********************************************
            * ������ �����־
            */
            if ( !flagClr ) {
                times++;
                if(times > 1)   //�ݴ�
                    flagClr = flags;
            }else{
               times = 0; 
            }
            
            /***********************************************
            * ������ �����־λ
            */
            OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.CommEvtFlagGrp,
                       ( OS_FLAGS      )flagClr,
                       ( OS_OPT        )OS_OPT_POST_FLAG_CLR,
                       ( OS_ERR       *)&err);
        }
        
        /***********************************************
        * ������ ����ʣ��ʱ��
        */
        dly   = CYCLE_TIME_TICKS - ( OSTimeGet(&err) - ticks );
        if ( dly  < 1 ) {
            dly = 1;
            OSTimeDlyHMSM(0, 0, 0, 1,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
                        //��ʱ��ִ����������
        } else if ( dly > CYCLE_TIME_TICKS ) {
            dly = CYCLE_TIME_TICKS;
        }
    }
}

/*******************************************************************************
* ��    �ƣ� APP_CommInit
* ��    �ܣ� �����ʼ��
* ��ڲ����� ��
* ���ڲ����� ��
* ��    �ߣ� wumingshen.
* �������ڣ� 2015-03-28
* ��    �ģ�
* �޸����ڣ�
*******************************************************************************/
void APP_CommInit(void)
{
    OS_ERR err;
    
        /***********************************************
    * ������ �����¼���־��,Э��comm�շ�
    */
    OSFlagCreate(( OS_FLAG_GRP  *)&Ctrl.Os.CommEvtFlagGrp,
                 ( CPU_CHAR     *)"App_CommFlag",
                 ( OS_FLAGS      )0,
                 ( OS_ERR       *)&err);
    
    Ctrl.Os.CommEvtFlag =     COMM_EVT_FLAG_MTR_RX              // MTR �����¼�
                            + COMM_EVT_FLAG_DTU_RX              // DTU �����¼�
                            + COMM_EVT_FLAG_TAX_RX              // OTR �����¼�
                            + COMM_EVT_FLAG_MTR_TX              
                            + COMM_EVT_FLAG_DTU_TX              
                            + COMM_EVT_FLAG_TAX_TX              
                            ;
    

    /***********************************************
    * ������ ��ʼ��MODBUSͨ��(����hook.c�д���)
    */        
    //App_ModbusInit();
    
    /***********************************************
    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
    */
    //OSRegWdtFlag(( OS_FLAGS     ) WDT_FLAG_COMM);
}

/*******************************************************************************
* 				end of file
*******************************************************************************/
#endif
