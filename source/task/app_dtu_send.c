/**************************************************************
* Description  : 处理板和DTU通讯，数据发送部分
* Author       : 2018/5/23 星期三, by redmorningcn
*/

/*******************************************************************************
* INCLUDES
*/
#include    <includes.h>
#include    <app_com_type.h>
#include    <app_dtu_send.h>


extern  void        app_ReadOneRecord(stcFlshRec *pRec,u32 num);

/**************************************************************
* Description  : 和DTU通讯，发送部分
* Author       : 2018/5/23 星期三, by redmorningcn
*/
void    app_dtu_send(void)
{
    u8  conntype;               //数据通许、参数设置、IAP
    u8  enablesend = 0;         //数据发送标识
    u8  replylen;
    u8  iapcode;
    //地址设置
    DtuCom->ConnCtrl.sCsnc.sourceaddr   = LKJ_MAINBOARD_ADDR;
    DtuCom->ConnCtrl.sCsnc.destaddr     = DTU_ADDR;
    DtuCom->ConnCtrl.sCsnc.framcode     = DtuCom->RxCtrl.sCsnc.framcode;
    DtuCom->ConnCtrl.sCsnc.framnum      = DtuCom->RxCtrl.sCsnc.framnum;     //将接收到数据发送
    
    conntype = DtuCom->ConnCtrl.ConnType;
    
    switch(conntype){
        /**************************************************************
        * Description  : 数据记录发送
        * Author       : 2018/5/23 星期三, by redmorningcn
        */
    case RECORD_SEND_COMM:     
        
        if(Ctrl.sRecNumMgr.Current == 0)
            return;
        
        //取数据记录号
        if(Ctrl.sRecNumMgr.Current < Ctrl.sRecNumMgr.GrsRead )  {
            Ctrl.sRecNumMgr.GrsRead = 0;
            if(Ctrl.sRecNumMgr.Current)
                Ctrl.sRecNumMgr.GrsRead = Ctrl.sRecNumMgr.Current - 1;        //最后有效记录赋值     
        }
        
        //读取数据记录
        app_ReadOneRecord((stcFlshRec *)&DtuCom->Wr,Ctrl.sRecNumMgr.GrsRead);     
        
        //数据记录按csnc协议打包 DataPackage_CSNC(strCsnrProtocolPara * sprotocolpara);
        DtuCom->ConnCtrl.sCsnc.framcode     = SET_FRAME_CODE;                   //新数据格式
        DtuCom->ConnCtrl.sCsnc.framnum      = DtuCom->ConnCtrl.SendRecordNum;   //发送记录号
        DtuCom->ConnCtrl.RecordSendFlg      = 1;        

        DtuCom->ConnCtrl.sCsnc.datalen      = sizeof(stcFlshRec);
        DtuCom->ConnCtrl.sCsnc.databuf      = (u8 *)&DtuCom->Wr;                //数据内容
        DtuCom->ConnCtrl.sCsnc.rxtxbuf      = DtuCom->pch->TxBuf;               //打包后的数据直接存入发送缓存
        
        DataPackage_CSNC((strCsnrProtocolPara *)&DtuCom->ConnCtrl.sCsnc);       //数据打包
        DtuCom->pch->TxBufByteCtr = DtuCom->ConnCtrl.sCsnc.rxtxlen;             //数据长度准备
        
        enablesend = 1;        //数据发送标识1
        break;  
        
        /**************************************************************
        * Description  : 参数读写（发送数据应答信号，应答接收到的内容，buf[8]状态位）
        * Author       : 2018/5/23 星期三, by redmorningcn
        */
    case SET_COMM:
        //数据记录按csnc协议打包 DataPackage_CSNC(strCsnrProtocolPara * sprotocolpara);
        switch(DtuCom->Rd.dtu.code){
        case CMD_RECORD_GET:
            DtuCom->ConnCtrl.sCsnc.framcode = SET_FRAME_CODE;                   //新数据格式
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
        DtuCom->ConnCtrl.sCsnc.datalen      = replylen;                         //应答长度

        DtuCom->ConnCtrl.sCsnc.databuf      = (u8 *)&DtuCom->Rd;                //数据内容
        DtuCom->ConnCtrl.sCsnc.rxtxbuf      = DtuCom->pch->TxBuf;               //打包后的数据直接存入发送缓存
        
        DataPackage_CSNC((strCsnrProtocolPara *)&DtuCom->ConnCtrl.sCsnc);       //数据打包
        DtuCom->pch->TxBufByteCtr = DtuCom->ConnCtrl.sCsnc.rxtxlen;             //数据长度准备
        
        enablesend = 1;                                                         //数据发送标识1
        DtuCom->ConnCtrl.ConnType = RECORD_SEND_COMM;                           //应答完后，更改通讯类型
        break;
        
        /**************************************************************
        * Description  : 程序升级。应答升级的内容
        * Author       : 2018/5/23 星期三, by redmorningcn
        */
    case IAP_COMM:
        iapcode = DtuCom->Rd.dtu.iap.code;  //IAP指令位
        
        switch(iapcode)
        {
        case IAP_DATA:                      //IAP数据传输应答
            DtuCom->ConnCtrl.sCsnc.datalen  = IAP_REPLY_DATA_LEN;
            break;
        case IAP_START:                     //IAP启动及结束应答
        case IAP_END:
        default:
            DtuCom->ConnCtrl.sCsnc.datalen  = DtuCom->RxCtrl.sCsnc.datalen;
            break;
        }
        
        DtuCom->ConnCtrl.sCsnc.databuf      = (u8 *)&DtuCom->Rd;                //数据内容
        DtuCom->ConnCtrl.sCsnc.rxtxbuf      = DtuCom->pch->TxBuf;               //打包后的数据直接存入发送缓存
        
        DataPackage_CSNC((strCsnrProtocolPara *)&DtuCom->ConnCtrl.sCsnc);       //数据打包
        DtuCom->pch->TxBufByteCtr = DtuCom->ConnCtrl.sCsnc.rxtxlen;             //数据长度准备
        enablesend = 1;                     //数据发送标识1
        if(iapcode == IAP_END)
            DtuCom->ConnCtrl.ConnType = RECORD_SEND_COMM;                       //默认状态位数据发送
        
        break;
        
    case GPS_COMM:                          //定位模块
        /**************************************************************
        * Description  : 查询模块的信号强度
        * Author       : 2018/6/4 星期一, by redmorningcn
        */
        strcpy((char *)DtuCom->pch->TxBuf,(const char *)GPS_RSSI_ASK);
        DtuCom->pch->TxBufByteCtr = strlen(GPS_RSSI_ASK);
        
        DtuCom->ConnCtrl.ConnType = RECORD_SEND_COMM;                           //默认状态位数据发送
        enablesend = 1;                     //数据发送标识1
        break;
        
    default:
        DtuCom->ConnCtrl.ConnType = RECORD_SEND_COMM;                           //默认状态位数据发送
        break;
    }
    
    if( enablesend == 1 )                   //有数据发送
    {
        //数据发送状态准备
        if(DtuCom->pch->TxBufByteCtr == 0)                                      //如果数据异常，发送20字节
            DtuCom->pch->TxBufByteCtr = 20;
        
        DtuCom->ConnCtrl.protocol   = CSNC_PROTOCOL;    
        DtuCom->ConnCtrl.RecvEndFlg = 0;                                        //如需要在规定时间内接收到应答是，协调处理。                                         
        DtuCom->RxCtrl.RecvFlg      = 0;
        //DtuCom->ConnCtrl.sCsnc
        
        //数据发送
        NMB_Tx(DtuCom->pch,DtuCom->pch->TxBuf,DtuCom->pch->TxBufByteCtr);
    }
}




