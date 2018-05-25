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


/**************************************************************
* Description  : ������������д��ת����
* Author       : 2018/5/25 ������, by redmorningcn
*/
void    app_operate_para(void)
{
    u32 code;       //ָ����
    
    code = DtuCom->Rd.dtu.code;
    
    switch(code)
    {
    case    CMD_TIME_SET:               //����ʱ��
        BSP_RTC_WriteTime(DtuCom->Rd.dtu.time);
        DtuCom->Rd.dtu.reply.ack = 1;   //��ʾ���óɹ�
        break;
    case    CMD_LOCO_SET:               //���û�����
        Ctrl.sProductInfo.LocoId.Nbr = DtuCom->Rd.dtu.loco.Nbr;
        Ctrl.sProductInfo.LocoId.Type= DtuCom->Rd.dtu.loco.Type;
        
        Ctrl.sRunPara.FramFlg.WrProduct = 1;                //�����洢����
        osal_set_event(OS_TASK_ID_STORE,OS_EVT_STORE_FRAM); //���ô洢�¼���������������
        
        DtuCom->Rd.dtu.reply.ack = 1;                       //��ʾ���óɹ�
        break;
    case    CMD_REC_CLR:                //��������
        Ctrl.sRecNumMgr.Current = 0;
        Ctrl.sRecNumMgr.GrsRead = 0;
        Ctrl.sRecNumMgr.PointNum= 0;
        Ctrl.sRecNumMgr.Record  = 0;
        
        Ctrl.sRunPara.FramFlg.WrNumMgr = 1;
        osal_set_event(OS_TASK_ID_STORE,OS_EVT_STORE_FRAM); //���ô洢�¼���������������

        break;
    case    CMD_SYS_RST:
        
        break;
    case    CMD_PARA_SET:
        
        break;
    case    CMD_PARA_GET:
        
        break;
    default:
        break;
    }
}


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
    if(DtuCom->RxCtrl.protocol == CSNC_PROTOCOL)            //csnc�첽����ͨѶЭ��
    {
        switch(DtuCom->RxCtrl.FrameCode)
        {
        case IAP_FRAME_CODE:
            DtuCom->ConnCtrl.ConnType = IAP_COMM;           //IAPͨѶ
            break;
        case SET_FRAME_CODE:
            DtuCom->ConnCtrl.ConnType = SET_COMM;           //������ȡ
            break;
        case RECORD_FRAME_CODE:
            if(DtuCom->ConnCtrl.RecordSendFlg == 1){         //�����ݷ��ͣ�ͨѶ���Ͳ��䡣
                DtuCom->ConnCtrl.RecordSendFlg = 0;
            }
            //break;
        default:
            DtuCom->ConnCtrl.ConnType = RECORD_SEND_COMM;   //Ĭ��Ϊ���ݷ���

            if(DtuCom->RxCtrl.sCsnc.sourceaddr == SET_ADDR)
            {
                DtuCom->ConnCtrl.ConnType = SET_COMM;           //������ȡ
            }
            
            break;
        }
    }
    
    //����ͨѶ���ʹ���
    conntype = DtuCom->ConnCtrl.ConnType;
    
    switch(conntype){
        
    case RECORD_SEND_COMM:     
        //���յ��������ݵ�Ӧ���жϷ���Ӧ��֡�źͽ���֡�ţ������Ϊ���ͳɹ���
        //�ж��Ƿ������ݷ��͡�
        enablesend = 0;
        if(DtuCom->ConnCtrl.SendRecordNum == DtuCom->RxCtrl.sCsnc.framnum)
        {
            Ctrl.sRecNumMgr.GrsRead++;
            DtuCom->ConnCtrl.SendRecordNum++;                                                //���ͼ�¼++
            Ctrl.sRunPara.FramFlg.WrNumMgr = 1;                             //���¼��ʶ��Ч
            osal_set_event(OS_TASK_ID_STORE,OS_EVT_STORE_FRAM);             //���ô洢�¼���������������
            
            if(Ctrl.sRecNumMgr.GrsRead < Ctrl.sRecNumMgr.Current)           //������δ���ͣ�����������������
            {
                enablesend = 1;         //�ٴ��������ݷ���    
            }
        }
        
        DtuCom->ConnCtrl.RecordSendFlg = 0;
        
        break;  
        
        /**************************************************************
        * Description  : ���ݼ�¼��ѯ������Ӧ��󣬽��������͸�Ϊ���ݷ��͡�
        * Author       : 2018/5/23 ������, by redmorningcn
        */
    case RECORD_GET_COMM:     
        
        DtuCom->ConnCtrl.ConnType = RECORD_SEND_COMM;
        DtuCom->ConnCtrl.RecordSendFlg = 0;
        
        enablesend = 0;                 //���跢������
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

        
        enablesend = 1;             //�������ݷ���
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




