/**************************************************************
* Description  : ������DTUͨѶ�����ݷ��Ͳ���
* Author       : 2018/5/23 ������, by redmorningcn
*/

/*******************************************************************************
* INCLUDES
*/
#include    <includes.h>
#include    <app_com_type.h>
#include    <app_dtu_send.h>


extern  void        app_ReadOneRecord(stcFlshRec *pRec,u32 num);

/**************************************************************
* Description  : ��DTUͨѶ�����Ͳ���
* Author       : 2018/5/23 ������, by redmorningcn
*/
void    app_dtu_send(StrCOMCtrl *Com)
{
    u8  conntype;               //����ͨ���������á�IAP
    u8  enablesend = 0;         //���ݷ��ͱ�ʶ
    u8  replylen;
    u8  iapcode;
    u32 recordnum;
    //��ַ����
    Com->ConnCtrl.sCsnc.sourceaddr   = LKJ_MAINBOARD_ADDR;
    Com->ConnCtrl.sCsnc.destaddr     = DTU_ADDR;
    Com->ConnCtrl.sCsnc.framcode     = Com->RxCtrl.sCsnc.framcode;
    Com->ConnCtrl.sCsnc.framnum      = Com->RxCtrl.sCsnc.framnum;     //�����յ����ݷ���
    
    conntype = Com->ConnCtrl.ConnType;
    
    switch(conntype){
        /**************************************************************
        * Description  : ���ݼ�¼����
        * Author       : 2018/5/23 ������, by redmorningcn
        */
    case RECORD_SEND_COMM:     
        
        //debug2018 
        if(Ctrl.sRecNumMgr.Current == 0)
            return;
        
        
            
        //ȡ���ݼ�¼��
        if(Ctrl.sRecNumMgr.Current <= Ctrl.sRecNumMgr.GrsRead )  {
            Ctrl.sRecNumMgr.GrsRead = 0;
            if(Ctrl.sRecNumMgr.Current)
                Ctrl.sRecNumMgr.GrsRead = Ctrl.sRecNumMgr.Current - 1;        //�����Ч��¼��ֵ     
        }
        
        //20180712 ����Ƕ�ȡָ�����ݣ��򲻽������ݶ�ȡ��ֱ�Ӷ�ָ������
        //��ȡ���ݼ�¼
        recordnum = Ctrl.sRecNumMgr.GrsRead;
        if(Ctrl.sRecNumMgr.PointNum ){
            recordnum = Ctrl.sRecNumMgr.PointNum;
            Ctrl.sRecNumMgr.PointNum = 0;
        }
        app_ReadOneRecord((stcFlshRec *)&Com->Wr,recordnum);     
        
        //���ݼ�¼��csncЭ���� DataPackage_CSNC(strCsnrProtocolPara * sprotocolpara);
        Com->ConnCtrl.sCsnc.framcode     = SET_FRAME_CODE;                   //�����ݸ�ʽ
        Com->ConnCtrl.sCsnc.framnum      = Com->ConnCtrl.SendRecordNum;     //���ͼ�¼��
        Com->ConnCtrl.RecordSendFlg      = 1;        

        Com->ConnCtrl.sCsnc.datalen      = sizeof(stcFlshRec);
        Com->ConnCtrl.sCsnc.databuf      = (u8 *)&Com->Wr;                  //��������
        Com->ConnCtrl.sCsnc.rxtxbuf      = Com->pch->TxBuf;                 //����������ֱ�Ӵ��뷢�ͻ���
        
        DataPackage_CSNC((strCsnrProtocolPara *)&Com->ConnCtrl.sCsnc);       //���ݴ��
        Com->pch->TxBufByteCtr = Com->ConnCtrl.sCsnc.rxtxlen;               //���ݳ���׼��
        
        enablesend = 1;        //���ݷ��ͱ�ʶ1
        break;  
        
        /**************************************************************
        * Description  : ������д����������Ӧ���źţ�Ӧ����յ������ݣ�buf[8]״̬λ��
        * Author       : 2018/5/23 ������, by redmorningcn
        */
    case SET_COMM:
        //���ݼ�¼��csncЭ���� DataPackage_CSNC(strCsnrProtocolPara * sprotocolpara);
        switch(Com->Rd.dtu.code){
        case CMD_RECORD_GET:
            Com->ConnCtrl.sCsnc.framcode = SET_FRAME_CODE;                   //�����ݸ�ʽ
            replylen                        = sizeof(stcFlshRec);
            break;
        case CMD_PARA_GET:
        case CMD_DETECT_GET:
            replylen    =       (u8)Com->Rd.dtu.paralen 
                            +   sizeof(Com->Rd.dtu.paralen ) 
                            +   sizeof(Com->Rd.dtu.paraaddr )
                            +   sizeof(Com->Rd.dtu.code );
            break;
        default:
            replylen    =   SET_REPLY_DATA_LEN;
            break;
        }
            
        Com->ConnCtrl.sCsnc.sourceaddr   = Com->RxCtrl.sCsnc.destaddr;
        Com->ConnCtrl.sCsnc.destaddr     = Com->RxCtrl.sCsnc.sourceaddr;
        Com->ConnCtrl.sCsnc.datalen      = replylen;                         //Ӧ�𳤶�

        Com->ConnCtrl.sCsnc.databuf      = (u8 *)&Com->Rd;                //��������
        Com->ConnCtrl.sCsnc.rxtxbuf      = Com->pch->TxBuf;               //����������ֱ�Ӵ��뷢�ͻ���
        
        DataPackage_CSNC((strCsnrProtocolPara *)&Com->ConnCtrl.sCsnc);       //���ݴ��
        Com->pch->TxBufByteCtr = Com->ConnCtrl.sCsnc.rxtxlen;             //���ݳ���׼��
        
        enablesend = 1;                                                         //���ݷ��ͱ�ʶ1
        Com->ConnCtrl.ConnType = RECORD_SEND_COMM;                           //Ӧ����󣬸���ͨѶ����
        break;
        
        /**************************************************************
        * Description  : ����������Ӧ������������
        * Author       : 2018/5/23 ������, by redmorningcn
        */
    case IAP_COMM:
        iapcode = Com->Rd.dtu.iap.code;  //IAPָ��λ
        
        switch(iapcode)
        {
        case IAP_DATA:                      //IAP���ݴ���Ӧ��
            Com->ConnCtrl.sCsnc.datalen  = IAP_REPLY_DATA_LEN;
            break;
        case IAP_START:                     //IAP����������Ӧ��
        case IAP_END:
        default:
            Com->ConnCtrl.sCsnc.datalen  = Com->RxCtrl.sCsnc.datalen;
            break;
        }
        
        Com->ConnCtrl.sCsnc.databuf      = (u8 *)&Com->Rd;                //��������
        Com->ConnCtrl.sCsnc.rxtxbuf      = Com->pch->TxBuf;               //����������ֱ�Ӵ��뷢�ͻ���
        
        DataPackage_CSNC((strCsnrProtocolPara *)&Com->ConnCtrl.sCsnc);       //���ݴ��
        Com->pch->TxBufByteCtr = Com->ConnCtrl.sCsnc.rxtxlen;             //���ݳ���׼��
        enablesend = 1;                     //���ݷ��ͱ�ʶ1
        if(iapcode == IAP_END)
            Com->ConnCtrl.ConnType = RECORD_SEND_COMM;                       //Ĭ��״̬λ���ݷ���
        
        break;
        
    case GPS_COMM:                          //��λģ��
//        //debug2018
//        return;
        /**************************************************************
        * Description  : ��ѯģ����ź�ǿ��
        * Author       : 2018/6/4 ����һ, by redmorningcn
        */
        strcpy((char *)Com->pch->TxBuf,(const char *)GPS_RSSI_ASK);
        Com->pch->TxBufByteCtr = strlen(GPS_RSSI_ASK);
        
        Com->ConnCtrl.ConnType = RECORD_SEND_COMM;                           //Ĭ��״̬λ���ݷ���
        enablesend = 1;                     //���ݷ��ͱ�ʶ1
        break;
        
    default:
        Com->ConnCtrl.ConnType = RECORD_SEND_COMM;                           //Ĭ��״̬λ���ݷ���
        break;
    }
    
    if( enablesend == 1 )                   //�����ݷ���
    {
        //���ݷ���״̬׼��
        if(Com->pch->TxBufByteCtr == 0)                                      //��������쳣������20�ֽ�
            Com->pch->TxBufByteCtr = 20;
        
        Com->ConnCtrl.protocol   = CSNC_PROTOCOL;    
        Com->ConnCtrl.RecvEndFlg = 0;                                        //����Ҫ�ڹ涨ʱ���ڽ��յ�Ӧ���ǣ�Э������                                         
        Com->RxCtrl.RecvFlg      = 0;
        //Com->ConnCtrl.sCsnc
        
        //���ݷ���
        NMB_Tx(Com->pch,Com->pch->TxBuf,Com->pch->TxBufByteCtr);
    }
}




