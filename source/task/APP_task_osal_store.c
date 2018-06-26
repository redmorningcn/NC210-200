/*******************************************************************************
* Description  : ���ݼ�¼�洢����ȡ����
* Author       : 2018/5/15 ���ڶ�, by redmorningcn
*******************************************************************************/


/*******************************************************************************
* INCLUDES
*/
#include <includes.h>
#include <string.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_store__c = "$Id: $";
#endif

#define APP_TASK_STORE_EN     DEF_ENABLED
#if APP_TASK_STORE_EN == DEF_ENABLED
/*******************************************************************************
* CONSTANTS
*/

/*******************************************************************************
* MACROS
*/
#define	START_EVT		        31
#define MAX_STORE_TYPE          10

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


/*******************************************************************************
* GLOBAL FUNCTIONS
*/

/*******************************************************************************
* EXTERN VARIABLES
*/


#if (UCOS_EN     == DEF_ENABLED)

#if OS_VERSION > 30000U
static  OS_SEM			Bsp_FlashSem;    	//�ź���
#else
static  OS_EVENT      *Bsp_FlashSem;       //�ź���
#endif

/*******************************************************************************
* ��    �ƣ� BSP_FlashWaitEvent
* ��    �ܣ� �ȴ��ź���
* ��ڲ����� ��
* ���ڲ����� 0���������󣩣�1�������ɹ���
* ��    �ߣ� redmorningcn
* �������ڣ� 2017-05-15
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ����ʹ��UCOS����ϵͳʱʹ��
*******************************************************************************/
u8 BSP_FlashOsInit(void)
{    
    /***********************************************
    * ������ OS�ӿ�
    */
#if (UCOS_EN     == DEF_ENABLED)
#if OS_VERSION > 30000U
    BSP_OS_SemCreate(&Bsp_FlashSem,1, "Bsp_FlashSem");      // �����ź���
#else
    Bsp_FramSem     = OSSemCreate(1);                       // �����ź���
#endif
#endif 
    return TRUE;
}

/*******************************************************************************
* ��    �ƣ� BSP_FlashWaitEvent
* ��    �ܣ� �ȴ��ź���
* ��ڲ����� ��
* ���ڲ����� 0���������󣩣�1�������ɹ���
* ��    �ߣ� redmorningcn
* �������ڣ� 2017-05-15
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ����ʹ��UCOS����ϵͳʱʹ��
*******************************************************************************/
static u8 BSP_FlashWaitEvent(void)
{
    /***********************************************
    * ������ OS�ӿ�
    */
    return BSP_OS_SemWait(&Bsp_FlashSem,0); 
}
/*******************************************************************************
* ��    �ƣ� FRAM_SendEvent
* ��    �ܣ� �ͷ��ź���
* ��ڲ����� ��
* ���ڲ����� ��
* ��    �ߣ� redmorningcn
* �������ڣ� 2017-05-15
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ����ʹ��UCOS����ϵͳʱʹ��
*******************************************************************************/
static void BSP_FlashSendEvent(void)
{
    BSP_OS_SemPost(&Bsp_FlashSem);                        	// �����ź���
}
#endif
/*******************************************************************************/

u8	    WriteFlshRec(u32 FlshAddr,stcFlshRec *sFlshRec);
void	ReadFlshRec(stcFlshRec * sFlshRec,u32 FlshRecNum);

/*******************************************************************************
* ��    �ƣ� GetRecFlashAddr
* ��    �ܣ� ȡ���ݼ�¼��ַ��
* ��ڲ����� �洢��¼��
* ���ڲ����� flash�洢��ַ
* ��    �ߣ� redmornigcn
* �������ڣ� 2017-05-15
* ��    �ģ�
* �޸����ڣ�
*******************************************************************************/
static u32  GetRecFlashAddr(u32 FlshRecNum)
{				
    return  (u32)(((FlshRecNum * sizeof(stcFlshRec)) % FLSH_MAX_SIZE)); 			
}

/*******************************************************************************
* Description  : ���ݼ�¼��ֵ�����������ڸ������и�ֵ��
* Author       : 2018/5/17 ������, by redmorningcn
*/
static  void    FmtRecord(void)
{
    
    if(Ctrl.sRunPara.SysSts.StartFlg    == 1){
        Ctrl.sRunPara.SysSts.StartFlg   = 0;
        Ctrl.Rec.EvtType =  START_EVT;                              //�����¼�
    }
    
    time_s t_tm  = TIME_GetCalendarTime();                          //ȡ��¼ʱ��
    
    Ctrl.Rec.Year   = t_tm.tm_year-2000;
    Ctrl.Rec.Mon    = t_tm.tm_mon;
    Ctrl.Rec.Day    = t_tm.tm_mday;	
    Ctrl.Rec.Hour   = t_tm.tm_hour;	
    Ctrl.Rec.Min    = t_tm.tm_min;	
    Ctrl.Rec.Sec    = t_tm.tm_sec;   
    
    Ctrl.Rec.LocoNbr = Ctrl.sProductInfo.LocoId.Nbr;                //������Ϣ
    Ctrl.Rec.LocoType= Ctrl.sProductInfo.LocoId.Type;                
    
}

/*******************************************************************************
* ��    �ƣ�RoadNum;        		StoreData
* ��    �ܣ�RelRoadNum;     		���ݴ洢���������ݼ�¼�Ž����ݴ洢��ָ��
StationNum;    		flash��ַ;���¼�¼�ŵ���Ϣ���󲿷����ݼ�¼��
E_StationNum;  		��������������ֱ�Ӷ�sCtrl.sRec �и��¡��ٲ������������ڸú����и��¡�
* ��ڲ�����SignalTyp;      	��
* ���ڲ�����LocoSign;       	��
* ��    ��        �� 	redmornigcn
* �������ڣ�LocoWorkState;  	2017-05-15
* ��    �ģ�LocoState;     
* �޸����ڣ�
*******************************************************************************/
u8 App_SaveRecord(void)
{	
    u32         addr ;	
    u16         CRC_sum1;
    u16         CRC_sum2;
    u16         CRC_sum3;
    u16         retrys;
    u8          ret;

    stcFlshRec  sRectmp;        //���ݼ�¼����ʱ�洢

    /*******************************************************************************
    * Description  : ���ü�¼ֵ
    * Author       : 2018/5/17 ������, by redmorningcn
    */
    FmtRecord();
    
    retrys = 3;                                                             //��������������
    
    do {
        /*******************************************************************************
        * Description  : ����flash��ַ��ѭ���洢
        * Author       : 2018/5/16 ������, by redmorningcn
        */
        addr = GetRecFlashAddr(Ctrl.sRecNumMgr.Current);

        memcpy((u8 *)&sRectmp,(u8 *)&Ctrl.Rec,sizeof(Ctrl.Rec));            //���ݿ�����ȫ�����ݱ��
        sRectmp.CrcCheck = GetCrc16Chk((u8*)&sRectmp,sizeof(stcFlshRec)-2); //����У��
        CRC_sum1 = sRectmp.CrcCheck;
        // ���ݴ洢��flash
        ret = WriteFlshRec(addr, (stcFlshRec *)&sRectmp);
        // ��FLASH�ж�ȡ�������жԱ�
        ReadFlshRec((stcFlshRec *)&sRectmp, Ctrl.sRecNumMgr.Current);
        /**************************************************
        * ������ ����У��
        */
        CRC_sum2 = GetCrc16Chk((u8*)&sRectmp,sizeof(stcFlshRec)-2);
        CRC_sum3 = sRectmp.CrcCheck; 
        
        if(CRC_sum1 == CRC_sum2 && CRC_sum1 == CRC_sum3) {                                          //���ݼ�¼��ȷ,��¼�������
            Ctrl.sRecNumMgr.Current++;          
            Ctrl.sRecNumMgr.Record++;
            Ctrl.Rec.RecordId = Ctrl.sRecNumMgr.Current;                
            break;
        }else{
            Ctrl.sRecNumMgr.Current++;                                      //��¼��ת  
            if( retrys == 2 )                                       
            {
                if(Ctrl.sRecNumMgr.Current % (4096/128) != 0)           //��תһҳ������һ�Ρ�
                    Ctrl.sRecNumMgr.Current++; 
            }
        }
    } while ( --retrys );

    if ( ret == DEF_FALSE || retrys == 0) {                             //��flash����
        Ctrl.sRunPara.Err.FlashErr     = FALSE;                         //D14=1���洢������
    }

    Ctrl.sRunPara.FramFlg.WrNumMgr = 1;         
    osal_set_event( OS_TASK_ID_STORE, OS_EVT_STORE_FRAM);               //֪ͨ�����FRAM

    return ret;
}

/*******************************************************************************
* ��    �ƣ� ReadFlshRec
* ��    �ܣ� ���ݼ�¼�ţ�ȡ���ݼ�¼
* ��ڲ����� ��
* ���ڲ����� ��
* ��    �ߣ� redmornigcn
* �������ڣ� 2017-05-15
* ��    �ģ�
* �޸����ڣ�
*******************************************************************************/
void	ReadFlshRec(stcFlshRec * sFlshRec,u32 FlshRecNum)
{
	u32		FlshAddr;
	BSP_FlashWaitEvent();
    
	FlshAddr = GetRecFlashAddr( FlshRecNum );
	
	ReadFlsh(FlshAddr,( INT08U * )sFlshRec,sizeof( stcFlshRec ));
	
	BSP_FlashSendEvent();
    
	return	;
}

/*******************************************************************************
* ��    �ƣ� ReadFlshRec
* ��    �ܣ� ���ݼ�¼�ţ�ȡ���ݼ�¼
* ��ڲ����� ��
* ���ڲ����� ��
* ��    �ߣ� redmornigcn
* �������ڣ� 2017-05-15
* ��    �ģ�
* �޸����ڣ�
*******************************************************************************/
u8	WriteFlshRec(u32 FlshAddr,stcFlshRec *sFlshRec)
{
	BSP_FlashWaitEvent();
    
	u8 ret = WriteFlsh(FlshAddr,(INT08U *)sFlshRec, sizeof(stcFlshRec));
	
	BSP_FlashSendEvent();
    
	return	ret;
}

/*******************************************************************************
* ��    �ƣ�  TaskInitStore
* ��    �ܣ�  �����ʼ��
* ��ڲ����� 	��
* ���ڲ����� 	��
* ��    �ߣ� 	redmornigcn
* �������ڣ� 	2017-05-15
* ��    �ģ�
* �޸����ڣ�
*/
void    BSP_StoreInit(void)
{    
    SPI_FLASH_Init();                       //��ʼ��mx25 spi�ӿ�
}

/*******************************************************************************
* Description  : �������
* Author       : 2018/5/16 ������, by redmorningcn
*/
void App_FramPara(void)
{
    int     add;
    
    if(Ctrl.sRunPara.FramFlg.Flags)                                        
    {
        /***********************************************************************
        * Description  : ��HEAD
        * Author       : 2018/5/16 ������, by redmorningcn
        */
        if(Ctrl.sRunPara.FramFlg.WrHead == 1)                               
        {
            Ctrl.sRunPara.FramFlg.WrHead = 0;
            add = (int)&Ctrl.sHeadInfo - (int)&Ctrl;                                
            
            WriteFM24CL64(add,(u8 *)&Ctrl.sHeadInfo,sizeof(Ctrl.sHeadInfo));  
        }
        /***********************************************************************
        * Description  : ��HEAD
        * Author       : 2018/5/16 ������, by redmorningcn
        */
        if(Ctrl.sRunPara.FramFlg.RdHead == 1)                               
        {
            Ctrl.sRunPara.FramFlg.RdHead = 0;
            add = (int)&Ctrl.sHeadInfo - (int)&Ctrl;                                
            
            ReadFM24CL64(add,(u8 *)&Ctrl.sHeadInfo,sizeof(Ctrl.sHeadInfo));  
        }
        
        /***********************************************************************
        * Description  : ��NumMgr
        * Author       : 2018/5/16 ������, by redmorningcn
        */
        if(Ctrl.sRunPara.FramFlg.WrNumMgr == 1)
        {
            Ctrl.sRunPara.FramFlg.WrNumMgr = 0;
            add = (int)&Ctrl.sRecNumMgr - (int)&Ctrl;
            
            WriteFM24CL64(add,(u8 *)&Ctrl.sRecNumMgr,sizeof(Ctrl.sRecNumMgr));
        }
        /***********************************************************************
        * Description  : ��NumMgr
        * Author       : 2018/5/16 ������, by redmorningcn
        */
        if(Ctrl.sRunPara.FramFlg.RdNumMgr == 1)
        {
            Ctrl.sRunPara.FramFlg.RdNumMgr = 0;
            add = (int)&Ctrl.sRecNumMgr - (int)&Ctrl;
            
            ReadFM24CL64(add,(u8 *)&Ctrl.sRecNumMgr,sizeof(Ctrl.sRecNumMgr));
        }
        
        /***********************************************************************
        * Description  : ���Ʒ��Ϣ
        * Author       : 2018/5/16 ������, by redmorningcn
        */
        if(Ctrl.sRunPara.FramFlg.WrProduct == 1)
        {
            Ctrl.sRunPara.FramFlg.WrProduct = 0;
            add = (int)&Ctrl.sProductInfo - (int)&Ctrl;
            
            WriteFM24CL64(add,(u8 *)&Ctrl.sProductInfo,sizeof(Ctrl.sProductInfo));
        }
        
        /***********************************************************************
        * Description  : ����Ʒ��Ϣ
        * Author       : 2018/5/16 ������, by redmorningcn
        */
        if(Ctrl.sRunPara.FramFlg.RdProduct == 1)
        {
            Ctrl.sRunPara.FramFlg.RdProduct = 0;
            add = (int)&Ctrl.sProductInfo - (int)&Ctrl;
            
            ReadFM24CL64(add,(u8 *)&Ctrl.sProductInfo,sizeof(Ctrl.sProductInfo));
        }        
        
        /***********************************************************************
        * Description  : ��������Ϣ
        * Author       : 2018/5/16 ������, by redmorningcn
        */
        if(Ctrl.sRunPara.FramFlg.WrRunPara == 1)
        {
            Ctrl.sRunPara.FramFlg.WrRunPara = 0;
            add = (int)&Ctrl.sProductInfo - (int)&Ctrl;
            
            WriteFM24CL64(add,(u8 *)&Ctrl.sRunPara,sizeof(Ctrl.sRunPara));
        } 
        
        /***********************************************************************
        * Description  : ��������Ϣ
        * Author       : 2018/5/16 ������, by redmorningcn
        */
        if(Ctrl.sRunPara.FramFlg.RdRunPara == 1)
        {
            Ctrl.sRunPara.FramFlg.RdRunPara = 0;
            add = (int)&Ctrl.sProductInfo - (int)&Ctrl;
            
            ReadFM24CL64(add,(u8 *)&Ctrl.sRunPara,sizeof(Ctrl.sRunPara));
        }    
    }
        
    Ctrl.sRunPara.FramFlg.Flags = 0;               //���ʶ
}

/*******************************************************************************
* ��    �ƣ� 		AppTaskStore
* ��    �ܣ� 		��������
* ��ڲ����� 	p_arg - �����񴴽���������
* ���ڲ����� 	��
* ��    �ߣ� 	redmorningcn.
* �������ڣ� 	2017-05-15
* ��    �ģ�
* �޸����ڣ�
*******************************************************************************/
osalEvt  TaskStoreEvtProcess(osalTid task_id, osalEvt task_event)
{
    /***********************************************
    * ������ �������Ź���־��λ
    */
    //OSSetWdtFlag(( OS_FLAGS     ) WDT_FLAG_STORE);
    
    /*******************************************************************************
    * Description  : �������ݼ�¼����Ctrl.sRunPara.StoreTimeʱ�����������ݱ��棩
    * Author       : 2018/5/15 ���ڶ�, by redmorningcn
    *******************************************************************************/
    if( task_event & OS_EVT_STORE_TICKS ) {
        osal_start_timerRl( OS_TASK_ID_STORE,
                            OS_EVT_STORE_TICKS,
                            OS_TICKS_PER_SEC * Ctrl.sRunPara.StoreTime);
        /***************************************************
        * ������ ��λ�������ݱ�־λ���������ݱ���FLASH
        */
        App_SaveRecord();                                       //�������ݼ�¼
                                                
        OS_ERR      err;
        OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.CommEvtFlagGrp,   //֪ͨDTU�����Խ������ݷ���
                   ( OS_FLAGS      )COMM_EVT_FLAG_DTU_TX,
                   ( OS_OPT        )OS_OPT_POST_FLAG_SET,
                   ( OS_ERR       *)&err);
        
        return ( task_event ^ OS_EVT_STORE_TICKS );
    }
    
    /*******************************************************************************
    * Description  : �������������ʶλ���档��ʶλ1�������Ӧ������
    * Author       : 2018/5/15 ���ڶ�, by redmorningcn
    *******************************************************************************/
    if( task_event & OS_EVT_STORE_FRAM ) {
        /***************************************************
        * ������ ��������洢���ȡ
        */
        App_FramPara();
        
        return ( task_event ^ OS_EVT_STORE_FRAM );
    }
    return 0;
}

/*******************************************************************************
* ��    �ƣ� TaskInitStore
* ��    �ܣ� �����ʼ��
* ��ڲ����� ��
* ���ڲ����� ��
* ��    �ߣ� redmornigcn
* �������ڣ� 2017-05-15
* ��    �ģ�
* �޸����ڣ�
*******************************************************************************/
void TaskInitStore(void)
{    
    /***********************************************
    * ������ ��ʼ��Flash�ײ���غ���
    */
    BSP_StoreInit();
    
//    /***********************************************
//    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
//    */
//    OSRegWdtFlag(( OS_FLAGS     )WDT_FLAG_STORE );
//    
    /***********************************************
    * ������ 2017/12/3,���������ô洢����Ϊ60��
    */
    osal_start_timerRl( OS_TASK_ID_STORE,
                        OS_EVT_STORE_TICKS,
                        //OS_TICKS_PER_SEC * 60);
                        OS_TICKS_PER_SEC * 6);
}


/**************************************************************
* Description  : ����ָ����¼�ţ���ȡ���ݼ�¼
������ݼ�¼��ȡ�������¸�ֵ������Ϣ�����ݼ�¼��Ϣ������״̬��Ϣ��У�顣
* Author       : 2018/5/23 ������, by redmorningcn
*/
void    app_ReadOneRecord(stcFlshRec *pRec,u32 num)
{
    u16         crc1;
    u16         crc2;
    u8          retrys = 2;
    
    do {
        ReadFlshRec(pRec, num); //��ȡָ����¼�ŵ�����
        
        crc1 = GetCrc16Chk((u8 *)pRec,sizeof(stcFlshRec)-2); 
        crc2 = pRec->CrcCheck;
        
        if(crc1 == crc2) {      //���У��ͨ�������ݶ�ȡ��ɣ��˳�
            return;
        }
    } while ( --retrys );
    
    /**************************************************
    * ������ ���¸�ֵ������Ϣ�����ݼ�¼��Ϣ������״̬��Ϣ��У��
    */ 
    
    pRec->RecordId    = num;         //��¼��
    pRec->LocoType    = Ctrl.sProductInfo.LocoId.Type  ;//�����ͺ�       2  
    pRec->LocoNbr     = Ctrl.sProductInfo.LocoId.Nbr   ;//�� �� ��       2 
    //pRec->Err.= 1;        //flash����
    crc1              = GetCrc16Chk((u8 *)pRec,sizeof(stcFlshRec)-2);
    pRec->CrcCheck    = crc1;
    
}

/*******************************************************************************
* 				                    end of file                                *
*******************************************************************************/
#endif
