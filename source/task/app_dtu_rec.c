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
#include    <string.h>

typedef  void (*pFunction)(void);			    //定义一个函数类型的参数.
pFunction   pApp;

/*******************************************************************************
 * 名    称： IAP_JumpTo()
 * 功    能： 跳转到应用程序段
 * 入口参数：
 * 出口参数： 无
 * 作　 　者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改： 
 * 修改日期： 
 *******************************************************************************/
void IAP_JumpTo(u32 appAddr)
{    
    u32     JumpAddress = 0;
    u8      cpu_sr;
        
    /***********************************************
    * 描述： 关中断，防止值被中断修改
    */
    CPU_CRITICAL_ENTER();

    /***********************************************
    * 描述： 获取应用入口及初始化堆栈指针
    */
    JumpAddress   =*(volatile u32*) (appAddr + 4); // 地址+4为PC地址
    pApp          = (pFunction)JumpAddress;         // 函数指针指向APP
//    __set_MSP       (*(volatile u32*) appAddr);    // 初始化主堆栈指针（MSP）
//    __set_PSP       (*(volatile u32*) appAddr);    // 初始化进程堆栈指针（PSP）
//    __set_CONTROL   (0);                            // 清零CONTROL
    /***********************************************
    * 描述： 跳转到APP程序
    */
    pApp();
    
    CPU_CRITICAL_EXIT();
}

/**************************************************************
* Description  : 参数操作（读写或转发）
* Author       : 2018/5/25 星期五, by redmorningcn
*/
void    app_operate_para(void)
{
    u32 code;       //指令码
    u32 recordnum;
    u16 data;
    u16 err;
    u16 addr;
    u8  len;

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
        
    case    CMD_REC_CLR:                                    //数据清零
        Ctrl.sRecNumMgr.Current = 0;
        Ctrl.sRecNumMgr.GrsRead = 0;
        Ctrl.sRecNumMgr.PointNum= 0;
        Ctrl.sRecNumMgr.Record  = 0;
        
        Ctrl.sRunPara.FramFlg.WrNumMgr = 1;
        osal_set_event(OS_TASK_ID_STORE,OS_EVT_STORE_FRAM); //设置存储事件，启动储存任务

        DtuCom->Rd.dtu.reply.ack = 1;                       //表示设置成功

        break;
        
    case    CMD_SYS_RST:                                    //系统重启
        //重启
        Ctrl.sRunPara.SysSts.SysReset = 1;                  //重启标识置位，准备重启
        
        DtuCom->Rd.dtu.reply.ack = 1;                       //表示设置成功

        //IAP_JumpTo(0);
        break;
        
    case    CMD_PARA_SET:                                   //参数设置 
        
            for(u16 i = 0; i < (DtuCom->Rd.dtu.paralen);i++ ){

                MB_HoldingRegWr (  (DtuCom->Rd.dtu.paraaddr)+i,
                                    DtuCom->Rd.dtu.parabuf[i],
                                    &err);   
                
                if(err != MODBUS_ERR_NONE){
                    DtuCom->Rd.dtu.reply.ack = 0;           //表示设置失败
                    break;
                }
            }
            DtuCom->Rd.dtu.reply.ack = 1;                   //表示设置成功

            break;
            
    case    CMD_PARA_GET:                                   //参数设置 
        //调用参数设置函数
        for(u16 i = 0; i < (DtuCom->Rd.dtu.paralen);i++ ){
            
            data = MB_HoldingRegRd((DtuCom->Rd.dtu.paraaddr)+i,&err);
            if(err != MODBUS_ERR_NONE){
                //DtuCom->Rd.dtu.reply.ack = 0;             //表示设置失败
                DtuCom->Rd.dtu.paralen = 0;
                break;
            }
            
            DtuCom->Rd.dtu.parabuf[i] = data;               //赋值
        }
        
        break;
        
    case    CMD_RECORD_GET:                                 //数据记录读取（读指定记录号的数据）
        recordnum = DtuCom->Rd.dtu.recordnum;
        //取数据记录号。
        if(Ctrl.sRecNumMgr.Current < recordnum)  {
            recordnum = 0;
            if(Ctrl.sRecNumMgr.Current)
                recordnum = Ctrl.sRecNumMgr.Current - 1;        //最后有效记录赋值     
        }
        //读取数据记录（查询指定的书记录）
        app_ReadOneRecord((stcFlshRec *)&DtuCom->Rd,recordnum); //读取数据记录，（Set应答信息从Rd发送）    
        
        break;
        
    case    CMD_DETECT_SET:                                     //写检测板数据记录
        MtrCom->ConnCtrl.MB_Node = (u8)DtuCom->Rd.dtu.paralen;  //低字节为node号
        len     = (u8)DtuCom->Rd.dtu.paralen >> 8;              //高字节为len
        
        if(len == 0)
            break;
        
        memcpy((u8 *)&MtrCom->Wr,DtuCom->Rd.dtu.parabuf,len);   //准备写入的数据
        addr    = (u8)DtuCom->Rd.dtu.paraaddr;
        
        extern  void    MtrWrSpecial(u16 addr,u8  len);
        MtrWrSpecial(addr,len);                                 //写入数据
        
        DtuCom->Rd.dtu.reply.ack = 0;
        if(MtrCom->ConnCtrl.datalen == len)                     //判断写入是否正确
            DtuCom->Rd.dtu.reply.ack = 1;                       //表示设置成功

        break;
        
    case    CMD_DETECT_GET:                                     //读检测板数据记录
        MtrCom->ConnCtrl.MB_Node = (u8)DtuCom->Rd.dtu.paralen;  //低字节为node号
        len     = (u8)DtuCom->Rd.dtu.paralen >> 8;              //高字节为len
        addr    = (u8)DtuCom->Rd.dtu.paraaddr;
        
        if(len == 0)
            break;
        
        extern  void    MtrRdSpecial(u16 addr,u8  len);
        MtrRdSpecial(addr,len);                                 //读入数据
        
        if(MtrCom->RxCtrl.Len == len)                           //判断写入是否正确
        {
            memcpy(DtuCom->Rd.dtu.parabuf,(u8 *)&MtrCom->Rd,len);//取出数据
        }else{
            DtuCom->Rd.dtu.paralen = 0;                         //将数据长度置0，表示错误
        }       
            
        break;        
    default:
        DtuCom->ConnCtrl.ConnType   = RECORD_SEND_COMM;         //默认为数据发送
        break;
    }
}

extern  void app_iap_deal(void);
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
    if(DtuCom->RxCtrl.protocol == CSNC_PROTOCOL)                //csnc异步串口通讯协议
    {
        switch(DtuCom->RxCtrl.FrameCode)
        {
        case IAP_FRAME_CODE:
            DtuCom->ConnCtrl.ConnType = IAP_COMM;               //IAP通讯
            break;
        case SET_FRAME_CODE:
            DtuCom->ConnCtrl.ConnType = SET_COMM;               //参数读取
            break;
        case RECORD_FRAME_CODE:
            if(DtuCom->ConnCtrl.RecordSendFlg == 1){            //有数据发送，通讯类型不变。
                DtuCom->ConnCtrl.RecordSendFlg = 0;
            }
            //break;
        default:
            DtuCom->ConnCtrl.ConnType       = RECORD_SEND_COMM; //默认为数据发送

            if(DtuCom->RxCtrl.sCsnc.sourceaddr == SET_ADDR)
            {
                DtuCom->ConnCtrl.ConnType   = SET_COMM;         //参数读取
            }
            
            break;
        }
    }
    
    /**************************************************************
    * Description  : 增加gps定位模块
    * Author       : 2018/6/4 星期一, by redmorningcn
    */
    if(DtuCom->RxCtrl.protocol == Q560_PROTOCOL){
        DtuCom->ConnCtrl.ConnType = GPS_COMM;                   //GPS定位模块通讯
    }

    //根据通讯类型处理
    conntype = DtuCom->ConnCtrl.ConnType;
    
    switch(conntype){
        
    case RECORD_SEND_COMM:     
        //接收到发送数据的应答。判断发送应答帧号和接收帧号，相等认为发送成功。
        //判断是否有数据发送。
        enablesend = 0;
        if(DtuCom->ConnCtrl.SendRecordNum == DtuCom->RxCtrl.sCsnc.framnum){
            Ctrl.sRecNumMgr.GrsRead++;
            DtuCom->ConnCtrl.SendRecordNum++;                               //发送记录++
            Ctrl.sRunPara.FramFlg.WrNumMgr = 1;                             //存记录标识有效
            osal_set_event(OS_TASK_ID_STORE,OS_EVT_STORE_FRAM);             //设置存储事件，启动储存任务
            
            if(Ctrl.sRecNumMgr.GrsRead < Ctrl.sRecNumMgr.Current)           //有数据未发送，继续启动发送任务
            {
                enablesend = 1;                                             //再次启动数据发送    
            }
        }
        
        DtuCom->ConnCtrl.RecordSendFlg = 0;
        
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
        
        app_iap_deal();                 //iap升级处理
        
        enablesend = 1;                 //启动数据发送
        break;
        
        
        /**************************************************************
        * Description  : GPS模块通讯,成功接收到数据
        * Author       : 2018/6/4 星期一, by redmorningcn
        */
    case GPS_COMM:
        
        enablesend = 0;                 
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




