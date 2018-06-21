/**************************************************************
* Description  : ������dtuͨѶ�����ղ���
* Author       : 2018/5/24 ������, by redmorningcn
*/

/*******************************************************************************
* INCLUDES
*/
#include    <includes.h>
#include    <app_com_type.h>
#include    <app_dtu_rec.h>
#include    <ds3231.h>
#include    <string.h>
#include    <bsp_time.h>

typedef  void (*pFunction)(void);			    //����һ���������͵Ĳ���.
pFunction   pApp;

/*******************************************************************************
 * ��    �ƣ� IAP_JumpTo()
 * ��    �ܣ� ��ת��Ӧ�ó����
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-04-23
 * ��    �ģ� 
 * �޸����ڣ� 
 *******************************************************************************/
void IAP_JumpTo(u32 appAddr)
{    
    u32     JumpAddress = 0;
    u8      cpu_sr;
        
    /***********************************************
    * ������ ���жϣ���ֵֹ���ж��޸�
    */
    CPU_CRITICAL_ENTER();

    /***********************************************
    * ������ ��ȡӦ����ڼ���ʼ����ջָ��
    */
    JumpAddress   =*(volatile u32*) (appAddr + 4); // ��ַ+4ΪPC��ַ
    pApp          = (pFunction)JumpAddress;         // ����ָ��ָ��APP
//    __set_MSP       (*(volatile u32*) appAddr);    // ��ʼ������ջָ�루MSP��
//    __set_PSP       (*(volatile u32*) appAddr);    // ��ʼ�����̶�ջָ�루PSP��
//    __set_CONTROL   (0);                            // ����CONTROL
    /***********************************************
    * ������ ��ת��APP����
    */
    pApp();
    
    CPU_CRITICAL_EXIT();
}

/**************************************************************
* Description  : ������������д��ת����
* Author       : 2018/5/25 ������, by redmorningcn
*/
void    app_operate_para(void)
{
    u32 code;       //ָ����
    u32 recordnum;
    u16 data;
    u16 err;
    u16 addr;
    u8  len;

    code = DtuCom->Rd.dtu.code;
    App_DispDelay(2000);

    switch(code)
    {
    case    CMD_TIME_SET:               //����ʱ��
        BSP_RTC_WriteTime(DtuCom->Rd.dtu.time);
        DtuCom->Rd.dtu.reply.ack = 1;   //��ʾ���óɹ�
        
        
        tm_now  = TIME_GetCalendarTime(); 

        uprintf("%02d-%02d-%02d",
                tm_now.tm_hour,
                tm_now.tm_min,
                tm_now.tm_sec);  
        break;
        
    case    CMD_LOCO_SET:               //���û�����
        Ctrl.sProductInfo.LocoId.Nbr = DtuCom->Rd.dtu.loco.Nbr;
        Ctrl.sProductInfo.LocoId.Type= DtuCom->Rd.dtu.loco.Type;
        
        Ctrl.sRunPara.FramFlg.WrProduct = 1;                //�����洢����
        osal_set_event(OS_TASK_ID_STORE,OS_EVT_STORE_FRAM); //���ô洢�¼���������������
        
        DtuCom->Rd.dtu.reply.ack = 1;                       //��ʾ���óɹ�
        
        
        uprintf("%4d.%4d",
        Ctrl.sProductInfo.LocoId.Type,
        Ctrl.sProductInfo.LocoId.Nbr); 
        break;
        
    case    CMD_REC_CLR:                                    //��������
        Ctrl.sRecNumMgr.Current = 0;
        Ctrl.sRecNumMgr.GrsRead = 0;
        Ctrl.sRecNumMgr.PointNum= 0;
        Ctrl.sRecNumMgr.Record  = 0;
        
        Ctrl.sRunPara.FramFlg.WrNumMgr = 1;
        osal_set_event(OS_TASK_ID_STORE,OS_EVT_STORE_FRAM); //���ô洢�¼���������������

        DtuCom->Rd.dtu.reply.ack = 1;                       //��ʾ���óɹ�
        
        uprintf("CALL"); 
        break;
        
    case    CMD_SYS_RST:                                    //ϵͳ����
        //����
        Ctrl.sRunPara.SysSts.SysReset = 1;                  //������ʶ��λ��׼������
        
        DtuCom->Rd.dtu.reply.ack = 1;                       //��ʾ���óɹ�

        //IAP_JumpTo(0);
        break;
        
    case    CMD_PARA_SET:                                   //�������� 
        
            for(u16 i = 0; i < (DtuCom->Rd.dtu.paralen) / 2;i++ ){

                MB_HoldingRegWr (  (DtuCom->Rd.dtu.paraaddr / 2)+i,
                                    DtuCom->Rd.dtu.parabuf[i],
                                    &err);   
                
                if(err != MODBUS_ERR_NONE){
                    DtuCom->Rd.dtu.reply.ack = 0;           //��ʾ����ʧ��
                    break;
                }
            }
            DtuCom->Rd.dtu.reply.ack = 1;                   //��ʾ���óɹ�

            break;
            
    case    CMD_PARA_GET:                                   //�������� 
        //���ò������ú���
        for(u16 i = 0; i < (DtuCom->Rd.dtu.paralen);i++ ){
            
            data = MB_HoldingRegRd((DtuCom->Rd.dtu.paraaddr)+i,&err);
            if(err != MODBUS_ERR_NONE){
                //DtuCom->Rd.dtu.reply.ack = 0;             //��ʾ����ʧ��
                DtuCom->Rd.dtu.paralen = 0;
                break;
            }
            
            DtuCom->Rd.dtu.parabuf[i] = data;               //��ֵ
        }
        
        break;
        
    case    CMD_RECORD_GET:                                 //���ݼ�¼��ȡ����ָ����¼�ŵ����ݣ�
        recordnum = DtuCom->Rd.dtu.recordnum;
        //ȡ���ݼ�¼�š�
        if(Ctrl.sRecNumMgr.Current < recordnum)  {
            recordnum = 0;
            if(Ctrl.sRecNumMgr.Current)
                recordnum = Ctrl.sRecNumMgr.Current - 1;        //�����Ч��¼��ֵ     
        }
        //��ȡ���ݼ�¼����ѯָ�������¼��
        app_ReadOneRecord((stcFlshRec *)&DtuCom->Rd,recordnum); //��ȡ���ݼ�¼����SetӦ����Ϣ��Rd���ͣ�    
        
        break;
        
    case    CMD_DETECT_SET:                                     //д�������ݼ�¼
        MtrCom->ConnCtrl.MB_Node = (u8)DtuCom->Rd.dtu.paralen;  //���ֽ�Ϊnode��
        len     = (u8)DtuCom->Rd.dtu.paralen >> 8;              //���ֽ�Ϊlen
        
        if(len == 0)
            break;
        
        memcpy((u8 *)&MtrCom->Wr,DtuCom->Rd.dtu.parabuf,len);   //׼��д�������
        addr    = (u8)DtuCom->Rd.dtu.paraaddr;
        
        extern  void    MtrWrSpecial(u16 addr,u8  len);
        MtrWrSpecial(addr,len);                                 //д������
        
        DtuCom->Rd.dtu.reply.ack = 0;
        if(MtrCom->ConnCtrl.datalen == len)                     //�ж�д���Ƿ���ȷ
            DtuCom->Rd.dtu.reply.ack = 1;                       //��ʾ���óɹ�

        break;
        
    case    CMD_DETECT_GET:                                     //���������ݼ�¼
        MtrCom->ConnCtrl.MB_Node = (u8)DtuCom->Rd.dtu.paralen;  //���ֽ�Ϊnode��
        len     = (u8)DtuCom->Rd.dtu.paralen >> 8;              //���ֽ�Ϊlen
        addr    = (u8)DtuCom->Rd.dtu.paraaddr;
        
        if(len == 0)
            break;
        
        extern  void    MtrRdSpecial(u16 addr,u8  len);
        MtrRdSpecial(addr,len);                                 //��������
        
        if(MtrCom->RxCtrl.Len == len)                           //�ж�д���Ƿ���ȷ
        {
            memcpy(DtuCom->Rd.dtu.parabuf,(u8 *)&MtrCom->Rd,len);//ȡ������
        }else{
            DtuCom->Rd.dtu.paralen = 0;                         //�����ݳ�����0����ʾ����
        }       
            
        break;        
    default:
        DtuCom->ConnCtrl.ConnType   = RECORD_SEND_COMM;         //Ĭ��Ϊ���ݷ���
        break;
    }
}

extern  void app_iap_deal(void);
/**************************************************************
* Description  : ��DTUͨѶ�����ղ���
* Author       : 2018/5/24 ������, by redmorningcn
*/
void    app_dtu_rec(void)
{
    u8  conntype;               //����ͨ���������á�IAP
    u8  enablesend = 0;         //���ݷ��ͱ�ʶ
    OS_ERR  err;
    
    /**************************************************************
    * Description  : ȷ��ͨѶ���͡�
    ����ǳ������أ����߲�����д�������Ϊ��Ӧ��ͨѶ���͡�
    * Author       : 2018/5/24 ������, by redmorningcn
    */
    if(DtuCom->RxCtrl.protocol == CSNC_PROTOCOL)                //csnc�첽����ͨѶЭ��
    {
        switch(DtuCom->RxCtrl.FrameCode)
        {
        case IAP_FRAME_CODE:
            DtuCom->ConnCtrl.ConnType = IAP_COMM;               //IAPͨѶ
            break;
        case SET_FRAME_CODE:
            DtuCom->ConnCtrl.ConnType = SET_COMM;               //������ȡ
            break;
        case RECORD_FRAME_CODE:
            if(DtuCom->ConnCtrl.RecordSendFlg == 1){            //�����ݷ��ͣ�ͨѶ���Ͳ��䡣
                DtuCom->ConnCtrl.RecordSendFlg = 0;
            }
            //break;
        default:
            DtuCom->ConnCtrl.ConnType       = RECORD_SEND_COMM; //Ĭ��Ϊ���ݷ���

            if(DtuCom->RxCtrl.sCsnc.sourceaddr == SET_ADDR)
            {
                DtuCom->ConnCtrl.ConnType   = SET_COMM;         //������ȡ
            }
            
            break;
        }
    }
    
    /**************************************************************
    * Description  : ����gps��λģ��
    * Author       : 2018/6/4 ����һ, by redmorningcn
    */
    if(DtuCom->RxCtrl.protocol == Q560_PROTOCOL){
        DtuCom->ConnCtrl.ConnType = GPS_COMM;                   //GPS��λģ��ͨѶ
    }

    //����ͨѶ���ʹ���
    conntype = DtuCom->ConnCtrl.ConnType;
    
    switch(conntype){
        
    case RECORD_SEND_COMM:     
        //���յ��������ݵ�Ӧ���жϷ���Ӧ��֡�źͽ���֡�ţ������Ϊ���ͳɹ���
        //�ж��Ƿ������ݷ��͡�
        enablesend = 0;
        if(DtuCom->ConnCtrl.SendRecordNum == DtuCom->RxCtrl.sCsnc.framnum){
            Ctrl.sRecNumMgr.GrsRead++;
            DtuCom->ConnCtrl.SendRecordNum++;                               //���ͼ�¼++
            Ctrl.sRunPara.FramFlg.WrNumMgr = 1;                             //���¼��ʶ��Ч
            osal_set_event(OS_TASK_ID_STORE,OS_EVT_STORE_FRAM);             //���ô洢�¼���������������
            
            if(Ctrl.sRecNumMgr.GrsRead < Ctrl.sRecNumMgr.Current)           //������δ���ͣ�����������������
            {
                enablesend = 1;                                             //�ٴ��������ݷ���    
            }
        }
        
        DtuCom->ConnCtrl.RecordSendFlg = 0;
        
        break;  
        
    case SET_COMM:
        
        app_operate_para();             //�������ã���ȡ��

        enablesend = 1;                 //�������ݷ���
        break;
        
        /**************************************************************
        * Description  : ����������Ӧ������������
        * Author       : 2018/5/23 ������, by redmorningcn
        */
    case IAP_COMM:
        
        app_iap_deal();                 //iap��������
        
        enablesend = 1;                 //�������ݷ���
        break;
        
        
        /**************************************************************
        * Description  : GPSģ��ͨѶ,�ɹ����յ�����
        * Author       : 2018/6/4 ����һ, by redmorningcn
        */
    case GPS_COMM:
        
        enablesend = 0;                 
        break;
    default:
        break;
    }
    
    if( enablesend == 1 ){
        OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.CommEvtFlagGrp,    //֪ͨDTU�����Խ������ݷ���
                   ( OS_FLAGS      )COMM_EVT_FLAG_DTU_TX,
                   ( OS_OPT        )OS_OPT_POST_FLAG_SET,
                   ( OS_ERR       *)&err
                       );
    }
}




