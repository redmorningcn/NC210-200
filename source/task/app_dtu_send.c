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
void    app_dtu_send(StrCOMCtrl *Com)
{
    u8  conntype;               //数据通许、参数设置、IAP
    u8  enablesend = 0;         //数据发送标识
    u8  replylen;
    u8  iapcode;
    u32 recordnum;
    static u16 gpsmode = 0;     
    //地址设置
    Com->ConnCtrl.sCsnc.sourceaddr   = LKJ_MAINBOARD_ADDR;
    Com->ConnCtrl.sCsnc.destaddr     = DTU_ADDR;
    Com->ConnCtrl.sCsnc.framcode     = Com->RxCtrl.sCsnc.framcode;
    Com->ConnCtrl.sCsnc.framnum      = Com->RxCtrl.sCsnc.framnum;     //将接收到数据发送
    
    conntype = Com->ConnCtrl.ConnType;
    
    switch(conntype){
        /**************************************************************
        * Description  : 数据记录发送
        * Author       : 2018/5/23 星期三, by redmorningcn
        */
    case RECORD_SEND_COMM:     
        
        if(Ctrl.sRunPara.SysSts.SetBitFlg )     //正在设置参数，不传输数据
            return;
        //debug2018 
        if(Ctrl.sRecNumMgr.Current == 0)
            return;
        
        //取数据记录号
        if(Ctrl.sRecNumMgr.Current <= Ctrl.sRecNumMgr.GrsRead )  {
            Ctrl.sRecNumMgr.GrsRead = 0;
            if(Ctrl.sRecNumMgr.Current)
                Ctrl.sRecNumMgr.GrsRead = Ctrl.sRecNumMgr.Current - 1;        //最后有效记录赋值     
        }
        
        //20180712 如果是读取指定数据，则不进行数据读取，直接读指定数据
        //读取数据记录
        recordnum = Ctrl.sRecNumMgr.GrsRead;
        if(Ctrl.sRecNumMgr.PointNum ){
            recordnum = Ctrl.sRecNumMgr.PointNum;
            Ctrl.sRecNumMgr.PointNum = 0;
        }
        app_ReadOneRecord((stcFlshRec *)&Com->Wr,recordnum);     
        
        //数据记录按csnc协议打包 DataPackage_CSNC(strCsnrProtocolPara * sprotocolpara);
        Com->ConnCtrl.sCsnc.framcode     = SET_FRAME_CODE;                  //新数据格式
        Com->ConnCtrl.sCsnc.framnum      = Com->ConnCtrl.SendRecordNum;     //发送记录号
        Com->ConnCtrl.RecordSendFlg      = 1;        

        Com->ConnCtrl.sCsnc.datalen      = sizeof(stcFlshRec);
        Com->ConnCtrl.sCsnc.databuf      = (u8 *)&Com->Wr;                  //数据内容
        Com->ConnCtrl.sCsnc.rxtxbuf      = Com->pch->TxBuf;                 //打包后的数据直接存入发送缓存
        
        DataPackage_CSNC((strCsnrProtocolPara *)&Com->ConnCtrl.sCsnc);       //数据打包
        Com->pch->TxBufByteCtr = Com->ConnCtrl.sCsnc.rxtxlen;               //数据长度准备
        
        enablesend = 1;        //数据发送标识1
        break;  
        
        /**************************************************************
        * Description  : 参数读写（发送数据应答信号，应答接收到的内容，buf[8]状态位）
        * Author       : 2018/5/23 星期三, by redmorningcn
        */
    case SET_COMM:
        //数据记录按csnc协议打包 DataPackage_CSNC(strCsnrProtocolPara * sprotocolpara);
        switch(Com->Rd.dtu.code){
        case CMD_RECORD_GET:
            Com->ConnCtrl.sCsnc.framcode = SET_FRAME_CODE;                   //新数据格式
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
        Com->ConnCtrl.sCsnc.datalen      = replylen;                         //应答长度

        Com->ConnCtrl.sCsnc.databuf      = (u8 *)&Com->Rd;                //数据内容
        Com->ConnCtrl.sCsnc.rxtxbuf      = Com->pch->TxBuf;               //打包后的数据直接存入发送缓存
        
        DataPackage_CSNC((strCsnrProtocolPara *)&Com->ConnCtrl.sCsnc);       //数据打包
        Com->pch->TxBufByteCtr = Com->ConnCtrl.sCsnc.rxtxlen;             //数据长度准备
        
        enablesend = 1;                                                         //数据发送标识1
        Com->ConnCtrl.ConnType = RECORD_SEND_COMM;                           //应答完后，更改通讯类型
        break;
        
        /**************************************************************
        * Description  : 程序升级。应答升级的内容
        * Author       : 2018/5/23 星期三, by redmorningcn
        */
    case IAP_COMM:
        iapcode = Com->Rd.dtu.iap.code;  //IAP指令位
        
        switch(iapcode)
        {
        case IAP_DATA:                      //IAP数据传输应答
            Com->ConnCtrl.sCsnc.datalen  = IAP_REPLY_DATA_LEN;
            break;
        case IAP_START:                     //IAP启动及结束应答
        case IAP_END:
        default:
            Com->ConnCtrl.sCsnc.datalen  = Com->RxCtrl.sCsnc.datalen;
            break;
        }
        
        Com->ConnCtrl.sCsnc.databuf      = (u8 *)&Com->Rd;                //数据内容
        Com->ConnCtrl.sCsnc.rxtxbuf      = Com->pch->TxBuf;               //打包后的数据直接存入发送缓存
        
        DataPackage_CSNC((strCsnrProtocolPara *)&Com->ConnCtrl.sCsnc);       //数据打包
        Com->pch->TxBufByteCtr = Com->ConnCtrl.sCsnc.rxtxlen;             //数据长度准备
        enablesend = 1;                     //数据发送标识1
        if(iapcode == IAP_END)
            Com->ConnCtrl.ConnType = RECORD_SEND_COMM;                       //默认状态位数据发送
        
        break;
        
    case GPS_COMM:                          //定位模块
//        //debug2018
//        return;
        
        /**************************************************************
        * Description  :  设置GPS模块工作模式
        * Author       : 2018/8/2 星期四, by redmorningcn
        */
        Ctrl.sRunPara.SetOutTimes = 10;     //
        
        switch(gpsmode %3)
        {
        case 0:
            if(gpsmode == 0){        //设置模块的工作模式
                
                /**************************************************************
                * Description  : 设置GPS信息只查询才能发送
                * Author       : 2018/8/2 星期四, by redmorningcn
                */
                //if(Ctrl.sRunPara.DevCfg.Flag.GpsEnFlag)
                {        //
                    strcpy((char *)Com->pch->TxBuf,(const char *)GPS_SCFG_SET);
                    Com->pch->TxBufByteCtr = strlen(GPS_SCFG_SET);
                    
                    Com->ConnCtrl.ConnType = RECORD_SEND_COMM;                       //默认状态位数据发送
                    enablesend = 1;  
                }
            }
            break;
        case 1:
            /**************************************************************
            * Description  : 请求GPS坐标信息
            * Author       : 2018/8/2 星期四, by redmorningcn
            */
            if(Ctrl.sRunPara.DevCfg.Flag.GpsEnFlag){                                //允许查询坐标信息，才发送查询指令

                strcpy((char *)Com->pch->TxBuf,(const char *)GPS_INFO_ASK);
                Com->pch->TxBufByteCtr = strlen(GPS_INFO_ASK);
                
                Com->ConnCtrl.ConnType = RECORD_SEND_COMM;                           //默认状态位数据发送
                enablesend = 1;   
            }
            break;
        case 2:
            /**************************************************************
            * Description  : 查询模块的信号强度
            * Author       : 2018/6/4 星期一, by redmorningcn
            */
            strcpy((char *)Com->pch->TxBuf,(const char *)GPS_RSSI_ASK);
            Com->pch->TxBufByteCtr = strlen(GPS_RSSI_ASK);
            
            Com->ConnCtrl.ConnType = RECORD_SEND_COMM;                           //默认状态位数据发送
            enablesend = 1;                                                     //数据发送标识1
            break;
        default:
            break;
      
        }
        gpsmode++;

        break;
        
    default:
        Com->ConnCtrl.ConnType = RECORD_SEND_COMM;                           //默认状态位数据发送
        break;
    }
    
    if( enablesend == 1 )                   //有数据发送
    {
        //数据发送状态准备
        if(Com->pch->TxBufByteCtr == 0)                                      //如果数据异常，发送20字节
            Com->pch->TxBufByteCtr = 20;
        
        Com->ConnCtrl.protocol   = CSNC_PROTOCOL;    
        Com->ConnCtrl.RecvEndFlg = 0;                                        //如需要在规定时间内接收到应答是，协调处理。                                         
        Com->RxCtrl.RecvFlg      = 0;
        //Com->ConnCtrl.sCsnc
        
        //数据发送
        NMB_Tx(Com->pch,Com->pch->TxBuf,Com->pch->TxBufByteCtr);
    }
}




