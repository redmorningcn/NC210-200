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
void    app_dtu_send(void)
{
    u8  conntype;               //����ͨ���������á�IAP
    u8  enablesend = 0;         //���ݷ��ͱ�ʶ
    u8  replylen;
    u8  iapcode;
    //��ַ����
    DtuCom->ConnCtrl.sCsnc.sourceaddr   = LKJ_MAINBOARD_ADDR;
    DtuCom->ConnCtrl.sCsnc.destaddr     = DTU_ADDR;
    DtuCom->ConnCtrl.sCsnc.framcode     = DtuCom->RxCtrl.sCsnc.framcode;
    DtuCom->ConnCtrl.sCsnc.framnum      = DtuCom->RxCtrl.sCsnc.framnum;     //�����յ����ݷ���
    
    conntype = DtuCom->ConnCtrl.ConnType;
    
    switch(conntype){
        /**************************************************************
        * Description  : ���ݼ�¼����
        * Author       : 2018/5/23 ������, by redmorningcn
        */
    case RECORD_SEND_COMM:     
        
        if(Ctrl.sRecNumMgr.Current == 0)
            return;
        
        //ȡ���ݼ�¼��
        if(Ctrl.sRecNumMgr.Current < Ctrl.sRecNumMgr.GrsRead )  {
            Ctrl.sRecNumMgr.GrsRead = 0;
            if(Ctrl.sRecNumMgr.Current)
                Ctrl.sRecNumMgr.GrsRead = Ctrl.sRecNumMgr.Current - 1;        //�����Ч��¼��ֵ     
        }
        
        //��ȡ���ݼ�¼
        app_ReadOneRecord((stcFlshRec *)&DtuCom->Wr,Ctrl.sRecNumMgr.GrsRead);     
        
        //���ݼ�¼��csncЭ���� DataPackage_CSNC(strCsnrProtocolPara * sprotocolpara);
        DtuCom->ConnCtrl.sCsnc.framcode     = SET_FRAME_CODE;                   //�����ݸ�ʽ
        DtuCom->ConnCtrl.sCsnc.framnum      = DtuCom->ConnCtrl.SendRecordNum;   //���ͼ�¼��
        DtuCom->ConnCtrl.RecordSendFlg      = 1;        

        DtuCom->ConnCtrl.sCsnc.datalen      = sizeof(stcFlshRec);
        DtuCom->ConnCtrl.sCsnc.databuf      = (u8 *)&DtuCom->Wr;                //��������
        DtuCom->ConnCtrl.sCsnc.rxtxbuf      = DtuCom->pch->TxBuf;               //����������ֱ�Ӵ��뷢�ͻ���
        
        DataPackage_CSNC((strCsnrProtocolPara *)&DtuCom->ConnCtrl.sCsnc);       //���ݴ��
        DtuCom->pch->TxBufByteCtr = DtuCom->ConnCtrl.sCsnc.rxtxlen;             //���ݳ���׼��
        
        enablesend = 1;        //���ݷ��ͱ�ʶ1
        break;  
        
        /**************************************************************
        * Description  : ������д����������Ӧ���źţ�Ӧ����յ������ݣ�buf[8]״̬λ��
        * Author       : 2018/5/23 ������, by redmorningcn
        */
    case SET_COMM:
        //���ݼ�¼��csncЭ���� DataPackage_CSNC(strCsnrProtocolPara * sprotocolpara);
        switch(DtuCom->Rd.dtu.code){
        case CMD_RECORD_GET:
            DtuCom->ConnCtrl.sCsnc.framcode = SET_FRAME_CODE;                   //�����ݸ�ʽ
            replylen                        = sizeof(stcFlshRec);
            break;
        case CMD_PARA_GET:
        case CMD_DETECT_GET:
            replylen    =       DtuCom->Rd.dtu.paralen 
                            +   sizeof(DtuCom->Rd.dtu.paralen ) 
                            +   sizeof(DtuCom->Rd.dtu.paraaddr )
                            +   sizeof(DtuCom->Rd.dtu.code );
            break;
        default:
            replylen    =   SET_REPLY_DATA_LEN;
            break;
        }
            
        DtuCom->ConnCtrl.sCsnc.sourceaddr   = DtuCom->RxCtrl.sCsnc.destaddr;
        DtuCom->ConnCtrl.sCsnc.destaddr     = DtuCom->RxCtrl.sCsnc.sourceaddr;
        DtuCom->ConnCtrl.sCsnc.datalen      = replylen;                         //Ӧ�𳤶�

        DtuCom->ConnCtrl.sCsnc.databuf      = (u8 *)&DtuCom->Rd;                //��������
        DtuCom->ConnCtrl.sCsnc.rxtxbuf      = DtuCom->pch->TxBuf;               //����������ֱ�Ӵ��뷢�ͻ���
        
        DataPackage_CSNC((strCsnrProtocolPara *)&DtuCom->ConnCtrl.sCsnc);       //���ݴ��
        DtuCom->pch->TxBufByteCtr = DtuCom->ConnCtrl.sCsnc.rxtxlen;             //���ݳ���׼��
        
        enablesend = 1;                                                         //���ݷ��ͱ�ʶ1
        DtuCom->ConnCtrl.ConnType = RECORD_SEND_COMM;                           //Ӧ����󣬸���ͨѶ����
        break;
        
        /**************************************************************
        * Description  : ����������Ӧ������������
        * Author       : 2018/5/23 ������, by redmorningcn
        */
    case IAP_COMM:
        iapcode = DtuCom->Rd.dtu.iap.code;  //IAPָ��λ
        
        switch(iapcode)
        {
        case IAP_DATA:                      //IAP���ݴ���Ӧ��
            DtuCom->ConnCtrl.sCsnc.datalen  = IAP_REPLY_DATA_LEN;
            break;
        case IAP_START:                     //IAP����������Ӧ��
        case IAP_END:
        default:
            DtuCom->ConnCtrl.sCsnc.datalen  = DtuCom->RxCtrl.sCsnc.datalen;
            break;
        }
        
        DtuCom->ConnCtrl.sCsnc.databuf      = (u8 *)&DtuCom->Rd;                //��������
        DtuCom->ConnCtrl.sCsnc.rxtxbuf      = DtuCom->pch->TxBuf;               //����������ֱ�Ӵ��뷢�ͻ���
        
        DataPackage_CSNC((strCsnrProtocolPara *)&DtuCom->ConnCtrl.sCsnc);       //���ݴ��
        DtuCom->pch->TxBufByteCtr = DtuCom->ConnCtrl.sCsnc.rxtxlen;             //���ݳ���׼��
        enablesend = 1;                     //���ݷ��ͱ�ʶ1
        if(iapcode == IAP_END)
            DtuCom->ConnCtrl.ConnType = RECORD_SEND_COMM;                       //Ĭ��״̬λ���ݷ���
        
        break;
        
    case GPS_COMM:                          //��λģ��
        /**************************************************************
        * Description  : ��ѯģ����ź�ǿ��
        * Author       : 2018/6/4 ����һ, by redmorningcn
        */
        strcpy((char *)DtuCom->pch->TxBuf,(const char *)GPS_RSSI_ASK);
        DtuCom->pch->TxBufByteCtr = strlen(GPS_RSSI_ASK);
        
        DtuCom->ConnCtrl.ConnType = RECORD_SEND_COMM;                           //Ĭ��״̬λ���ݷ���
        enablesend = 1;                     //���ݷ��ͱ�ʶ1
        break;
        
    default:
        DtuCom->ConnCtrl.ConnType = RECORD_SEND_COMM;                           //Ĭ��״̬λ���ݷ���
        break;
    }
    
    if( enablesend == 1 )                   //�����ݷ���
    {
        //���ݷ���״̬׼��
        if(DtuCom->pch->TxBufByteCtr == 0)                                      //��������쳣������20�ֽ�
            DtuCom->pch->TxBufByteCtr = 20;
        
        DtuCom->ConnCtrl.protocol   = CSNC_PROTOCOL;    
        DtuCom->ConnCtrl.RecvEndFlg = 0;                                        //����Ҫ�ڹ涨ʱ���ڽ��յ�Ӧ���ǣ�Э������                                         
        DtuCom->RxCtrl.RecvFlg      = 0;
        //DtuCom->ConnCtrl.sCsnc
        
        //���ݷ���
        NMB_Tx(DtuCom->pch,DtuCom->pch->TxBuf,DtuCom->pch->TxBufByteCtr);
    }
}




