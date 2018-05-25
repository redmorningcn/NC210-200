/**************************************************************
* Description  : 处理板和dtu通讯，接收部分
* Author       : 2018/5/24 星期四, by redmorningcn
*/

/*******************************************************************************
* INCLUDES
*/
#include    <includes.h>
#include    <app_com_type.h>
#include    <app_dtu_rec.h>
#include    <ds3231.h>


/**************************************************************
* Description  : 参数操作（读写或转发）
* Author       : 2018/5/25 星期五, by redmorningcn
*/
void    app_operate_para(void)
{
    u32 code;       //指令码
    
    code = DtuCom->Rd.dtu.code;
    
    switch(code)
    {
    case    CMD_TIME_SET:               //设置时间
        BSP_RTC_WriteTime(DtuCom->Rd.dtu.time);
        DtuCom->Rd.dtu.reply.ack = 1;   //表示设置成功
        break;
    case    CMD_LOCO_SET:               //设置机车号
        Ctrl.sProductInfo.LocoId.Nbr = DtuCom->Rd.dtu.loco.Nbr;
        Ctrl.sProductInfo.LocoId.Type= DtuCom->Rd.dtu.loco.Type;
        
        Ctrl.sRunPara.FramFlg.WrProduct = 1;                //启动存储任务。
        osal_set_event(OS_TASK_ID_STORE,OS_EVT_STORE_FRAM); //设置存储事件，启动储存任务
        
        DtuCom->Rd.dtu.reply.ack = 1;                       //表示设置成功
        break;
    case    CMD_REC_CLR:                //数据清零
        Ctrl.sRecNumMgr.Current = 0;
        Ctrl.sRecNumMgr.GrsRead = 0;
        Ctrl.sRecNumMgr.PointNum= 0;
        Ctrl.sRecNumMgr.Record  = 0;
        
        Ctrl.sRunPara.FramFlg.WrNumMgr = 1;
        osal_set_event(OS_TASK_ID_STORE,OS_EVT_STORE_FRAM); //设置存储事件，启动储存任务

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
* Description  : 和DTU通讯，接收部分
* Author       : 2018/5/24 星期三, by redmorningcn
*/
void    app_dtu_rec(void)
{
    u8  conntype;               //数据通许、参数设置、IAP
    u8  enablesend = 0;         //数据发送标识
    OS_ERR  err;
    
    /**************************************************************
    * Description  : 确定通讯类型。
    如果是程序下载，或者参数读写，则更改为对应的通讯类型。
    * Author       : 2018/5/24 星期四, by redmorningcn
    */
    if(DtuCom->RxCtrl.protocol == CSNC_PROTOCOL)            //csnc异步串口通讯协议
    {
        switch(DtuCom->RxCtrl.FrameCode)
        {
        case IAP_FRAME_CODE:
            DtuCom->ConnCtrl.ConnType = IAP_COMM;           //IAP通讯
            break;
        case SET_FRAME_CODE:
            DtuCom->ConnCtrl.ConnType = SET_COMM;           //参数读取
            break;
        case RECORD_FRAME_CODE:
            if(DtuCom->ConnCtrl.RecordSendFlg == 1){         //有数据发送，通讯类型不变。
                DtuCom->ConnCtrl.RecordSendFlg = 0;
            }
            //break;
        default:
            DtuCom->ConnCtrl.ConnType = RECORD_SEND_COMM;   //默认为数据发送

            if(DtuCom->RxCtrl.sCsnc.sourceaddr == SET_ADDR)
            {
                DtuCom->ConnCtrl.ConnType = SET_COMM;           //参数读取
            }
            
            break;
        }
    }
    
    //更具通讯类型处理
    conntype = DtuCom->ConnCtrl.ConnType;
    
    switch(conntype){
        
    case RECORD_SEND_COMM:     
        //接收到发送数据的应答。判断发送应答帧号和接收帧号，相等认为发送成功。
        //判断是否有数据发送。
        enablesend = 0;
        if(DtuCom->ConnCtrl.SendRecordNum == DtuCom->RxCtrl.sCsnc.framnum)
        {
            Ctrl.sRecNumMgr.GrsRead++;
            DtuCom->ConnCtrl.SendRecordNum++;                                                //发送记录++
            Ctrl.sRunPara.FramFlg.WrNumMgr = 1;                             //存记录标识有效
            osal_set_event(OS_TASK_ID_STORE,OS_EVT_STORE_FRAM);             //设置存储事件，启动储存任务
            
            if(Ctrl.sRecNumMgr.GrsRead < Ctrl.sRecNumMgr.Current)           //有数据未发送，继续启动发送任务
            {
                enablesend = 1;         //再次启动数据发送    
            }
        }
        
        DtuCom->ConnCtrl.RecordSendFlg = 0;
        
        break;  
        
        /**************************************************************
        * Description  : 数据记录查询。接收应答后，将连接类型改为数据发送。
        * Author       : 2018/5/23 星期三, by redmorningcn
        */
    case RECORD_GET_COMM:     
        
        DtuCom->ConnCtrl.ConnType = RECORD_SEND_COMM;
        DtuCom->ConnCtrl.RecordSendFlg = 0;
        
        enablesend = 0;                 //不需发送数据
        break;  
        

    case SET_COMM:
        
        app_operate_para();             //参数设置（读取）

        enablesend = 1;                 //启动数据发送
        break;
        
        /**************************************************************
        * Description  : 程序升级。应答升级的内容
        * Author       : 2018/5/23 星期三, by redmorningcn
        */
    case IAP_COMM:

        
        enablesend = 1;             //启动数据发送
        break;
        
        
    default:
        break;
    }
    
    if( enablesend == 1 ){
        OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.CommEvtFlagGrp,    //通知DTU，可以进行数据发送
                   ( OS_FLAGS      )COMM_EVT_FLAG_DTU_TX,
                   ( OS_OPT        )OS_OPT_POST_FLAG_SET,
                   ( OS_ERR       *)&err
                       );
    }
}




