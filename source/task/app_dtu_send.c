/**************************************************************
* Description  : �������DTUͨѶ�����ݷ��Ͳ���
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
    u8  conntype;               //����ͨ�����������á�IAP
    u8  enablesend = 0;         //���ݷ��ͱ�ʶ
    u8  code; 
    
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
        
        //ȡ���ݼ�¼��
        if(Ctrl.sRecNumMgr.Current < Ctrl.sRecNumMgr.GrsRead )  {
            Ctrl.sRecNumMgr.GrsRead = 0;
            if(Ctrl.sRecNumMgr.Current)
                Ctrl.sRecNumMgr.GrsRead = Ctrl.sRecNumMgr.Current - 1;        //�����Ч��¼��ֵ     
        }
        
        //��ȡ���ݼ�¼
        app_ReadOneRecord((stcFlshRec *)&DtuCom->Wr,Ctrl.sRecNumMgr.GrsRead);     
        
        //���ݼ�¼��csncЭ���� DataPackage_CSNC(strCsnrProtocolPara * sprotocolpara);
        DtuCom->ConnCtrl.sCsnc.framnum      = DtuCom->ConnCtrl.SendRecordNum;   //���ͼ�¼��
        
        DtuCom->ConnCtrl.sCsnc.datalen      = sizeof(stcFlshRec);
        DtuCom->ConnCtrl.sCsnc.databuf      = (u8 *)&DtuCom->Wr;                //��������
        DtuCom->ConnCtrl.sCsnc.rxtxbuf      = DtuCom->pch->TxBuf;               //����������ֱ�Ӵ��뷢�ͻ���
        
        DataPackage_CSNC((strCsnrProtocolPara *)&DtuCom->ConnCtrl.sCsnc);       //���ݴ��
        DtuCom->pch->TxBufByteCtr = DtuCom->ConnCtrl.sCsnc.rxtxlen;             //���ݳ���׼��
        
        enablesend = 1;        //���ݷ��ͱ�ʶ1
        break;  
        
        /**************************************************************
        * Description  : ���ݼ�¼��ѯ
        * Author       : 2018/5/23 ������, by redmorningcn
        */
    case RECORD_GET_COMM:     
        
        //ȡ���ݼ�¼�š�
        if(Ctrl.sRecNumMgr.Current < Ctrl.sRecNumMgr.PointNum )  {
            Ctrl.sRecNumMgr.PointNum = 0;
            if(Ctrl.sRecNumMgr.Current)
                Ctrl.sRecNumMgr.PointNum = Ctrl.sRecNumMgr.Current - 1;        //�����Ч��¼��ֵ     
        }
        
        //��ȡ���ݼ�¼����ѯָ�������¼��
        app_ReadOneRecord((stcFlshRec *)&DtuCom->Wr,Ctrl.sRecNumMgr.PointNum); //    
        
        //���ݼ�¼��csncЭ���� DataPackage_CSNC(strCsnrProtocolPara * sprotocolpara);
        DtuCom->ConnCtrl.sCsnc.framnum      = DtuCom->ConnCtrl.SendRecordNum;   //���ͼ�¼��
        
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

        DtuCom->ConnCtrl.sCsnc.datalen      = SET_REPLY_DATA_LEN;
        DtuCom->ConnCtrl.sCsnc.databuf      = (u8 *)&DtuCom->Rd;                //��������
        DtuCom->ConnCtrl.sCsnc.rxtxbuf      = DtuCom->pch->TxBuf;               //����������ֱ�Ӵ��뷢�ͻ���
        
        DataPackage_CSNC((strCsnrProtocolPara *)&DtuCom->ConnCtrl.sCsnc);       //���ݴ��
        DtuCom->pch->TxBufByteCtr = DtuCom->ConnCtrl.sCsnc.rxtxlen;             //���ݳ���׼��
        
        enablesend = 1;     //���ݷ��ͱ�ʶ1
        break;
        
        /**************************************************************
        * Description  : ����������Ӧ������������
        * Author       : 2018/5/23 ������, by redmorningcn
        */
    case IAP_COMM:
        code = DtuCom->Rd.Buf[0];       //IAPָ��λ
        switch(code)
        {
//        case IAP_DATA:                  //IAP���ݴ���Ӧ��
//            DtuCom->ConnCtrl.sCsnc.datalen  = IAP_REPLY_DATA_LEN;
//
//            break;
//        case IAP_START:                 //IAP����������Ӧ��
//        case IAP_END:
//        default:
//            DtuCom->ConnCtrl.sCsnc.datalen  = DtuCom->RxCtrl.sCsnc.datalen;
//            break;
        }
        
        DtuCom->ConnCtrl.sCsnc.databuf      = (u8 *)&DtuCom->Rd;                //��������
        DtuCom->ConnCtrl.sCsnc.rxtxbuf      = DtuCom->pch->TxBuf;               //����������ֱ�Ӵ��뷢�ͻ���
        
        DataPackage_CSNC((strCsnrProtocolPara *)&DtuCom->ConnCtrl.sCsnc);       //���ݴ��
        DtuCom->pch->TxBufByteCtr = DtuCom->ConnCtrl.sCsnc.rxtxlen;             //���ݳ���׼��
        enablesend = 1;         //���ݷ��ͱ�ʶ1
        
        break;
        

    default:
        break;
    }
    
    if( enablesend == 1 )        //�����ݷ���
    {
        //���ݷ���״̬׼��
        if(DtuCom->pch->TxBufByteCtr == 0)                                      //��������쳣������20�ֽ�
            DtuCom->pch->TxBufByteCtr = 20;
        DtuCom->ConnCtrl.protocol   = CSNC_PROTOCOL;    
        DtuCom->ConnCtrl.SendFlg    = 1;
        DtuCom->ConnCtrl.RecvEndFlg = 0;                                        //����Ҫ�ڹ涨ʱ���ڽ��յ�Ӧ���ǣ�Э��������                                         
        DtuCom->RxCtrl.RecvFlg      = 0;
        //DtuCom->ConnCtrl.sCsnc
        
        //���ݷ���
        NMB_Tx(DtuCom->pch,DtuCom->pch->TxBuf,DtuCom->pch->TxBufByteCtr);
    }
}



