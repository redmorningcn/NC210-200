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
#include    <bsp_time.h>

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
void    app_operate_para(StrCOMCtrl *Com)
{
    u32 code;       //指令码
    u32 recordnum;
    u16 data;
    u16 err;
    u16 addr;
    u8  len;
    u8  node;
    
    code = Com->Rd.dtu.code;

    switch(code)
    {
    case    CMD_TIME_SET:               //设置时间
        BSP_RTC_WriteTime(Com->Rd.dtu.time);
        Com->Rd.dtu.reply.ack = 1;   //表示设置成功
        
        
        tm_now  = TIME_GetCalendarTime(); 

        App_DispDelay(2000);

        uprintf("%02d-%02d-%02d",
                tm_now.tm_hour,
                tm_now.tm_min,
                tm_now.tm_sec);  
        break;
        
    case    CMD_LOCO_SET:               //设置机车号
        Ctrl.sProductInfo.LocoId.Nbr = Com->Rd.dtu.loco.Nbr;
        Ctrl.sProductInfo.LocoId.Type= Com->Rd.dtu.loco.Type;
        
        Ctrl.sRunPara.FramFlg.WrProduct = 1;                //启动存储任务。
        osal_set_event(OS_TASK_ID_STORE,OS_EVT_STORE_FRAM); //设置存储事件，启动储存任务
        
        Com->Rd.dtu.reply.ack = 1;                       //表示设置成功
        
        App_DispDelay(2000);

        uprintf("%4d.%4d",
        Ctrl.sProductInfo.LocoId.Type,
        Ctrl.sProductInfo.LocoId.Nbr); 
        break;
        
    case    CMD_REC_CLR:                                    //数据清零
        Ctrl.sRecNumMgr.Current = 0;
        Ctrl.sRecNumMgr.GrsRead = 0;
        Ctrl.sRecNumMgr.PointNum= 0;
        Ctrl.sRecNumMgr.Record  = 0;
        
        Ctrl.sRunPara.FramFlg.WrNumMgr = 1;
        osal_set_event(OS_TASK_ID_STORE,OS_EVT_STORE_FRAM); //设置存储事件，启动储存任务

        Com->Rd.dtu.reply.ack = 1;                       //表示设置成功
        
        App_DispDelay(2000);

        uprintf("CALL"); 
        break;
        
    case    CMD_SYS_RST:                                    //系统重启
        //重启
        Ctrl.sRunPara.SysSts.SysReset = 1;                  //重启标识置位，准备重启
        
        Com->Rd.dtu.reply.ack = 1;                          //表示设置成功
        
        App_DispDelay(2000);
        uprintf("rst...");                                  //系统准备重启    

        break;
        
    case    CMD_PARA_SET:                                   //参数设置 
        
            for(u16 i = 0; i < (Com->Rd.dtu.paralen) / 2;i++ ){

                MB_HoldingRegWr (  (Com->Rd.dtu.paraaddr / 2)+i,
                                    Com->Rd.dtu.parabuf[i],
                                    &err);   
                
                if(err != MODBUS_ERR_NONE){
                    Com->Rd.dtu.reply.ack = 0;           //表示设置失败
                    break;
                }
                
                osal_set_event(OS_TASK_ID_STORE,OS_EVT_STORE_FRAM); //设置存储事件，启动储存任务
            }
            Com->Rd.dtu.reply.ack = 1;                   //表示设置成功

            break;
            
    case    CMD_PARA_GET:                                   //参数设置 
        
        
        if((u8)Com->Rd.dtu.paralen > sizeof(Com->Rd.dtu.parabuf)){
            Com->Rd.dtu.paralen = 0;
            break;
        }
        
        
        //调用参数设置函数
        for(u16 i = 0; i < (Com->Rd.dtu.paralen)/2;i++ ){
            
            data = MB_HoldingRegRd((Com->Rd.dtu.paraaddr/2)+i,&err);
            if(err != MODBUS_ERR_NONE){
                //Com->Rd.dtu.reply.ack = 0;             //表示设置失败
                Com->Rd.dtu.paralen = 0;
                break;
            }
            
            Com->Rd.dtu.parabuf[i] = data;               //赋值
        }
        
        break;
        
    case    CMD_RECORD_GET:                                     //数据记录读取（读指定记录号的数据）
        recordnum = Com->Rd.dtu.recordnum;
        Ctrl.sRecNumMgr.PointNum = recordnum;
        
        //取数据记录号。
        //if(Ctrl.sRecNumMgr.Current < recordnum)  {
         //   recordnum = 0;
        //    if(Ctrl.sRecNumMgr.Current)
       //         recordnum = Ctrl.sRecNumMgr.Current - 1;        //最后有效记录赋值     
        //}
        //读取数据记录（查询指定的书记录）
        //app_ReadOneRecord((stcFlshRec *)&Com->Rd,recordnum);    //读取数据记录，（Set应答信息从Rd发送）    
        
        Ctrl.sRunPara.SysSts.SetBitFlg      = 0;                //不是参数设置状态
        Com->ConnCtrl.ConnType              = RECORD_SEND_COMM; //默认为数据发送        
        break;
        
    case    CMD_DETECT_SET:                                     //写检测板数据记录
        node    = Com->Rd.dtu.paralen >> 8;                     //取node
        MtrCom->ConnCtrl.MB_Node = node;                        //低字节为node号
        //len     = (u8)Com->Rd.dtu.paralen >> 8;               //高字节为len
        len     = (u8)Com->Rd.dtu.paralen ;                     //高字节为len
        if(len == 0)
            break;
        
        memcpy((u8 *)&MtrCom->Wr,Com->Rd.dtu.parabuf,len);      //准备写入的数据
        addr    = Com->Rd.dtu.paraaddr;
        
        extern  void    MtrWrSpecial(u16 addr,u8  len);
        MtrWrSpecial(addr/2,len/2);                              //写入数据 (16位计算)
        
        Com->Rd.dtu.reply.ack = 0;
        if(MtrCom->ConnCtrl.datalen == len/2)                     //判断写入是否正确
            Com->Rd.dtu.reply.ack = 1;                       //表示设置成功

        break;
        
    case    CMD_DETECT_GET:                                     //读检测板数据记录
        //MtrCom->ConnCtrl.MB_Node = (u8)Com->Rd.dtu.paralen;  //低字节为node号
        //len     = (u8)Com->Rd.dtu.paralen >> 8;              //高字节为len
        
        node    = Com->Rd.dtu.paralen >> 8;                  //取node
        MtrCom->ConnCtrl.MB_Node = node;  //低字节为node号
        //len     = (u8)Com->Rd.dtu.paralen >> 8;              //高字节为len
        len     = (u8)Com->Rd.dtu.paralen ;                  //高字节为len
        addr    = Com->Rd.dtu.paraaddr;
        
        if(len == 0)
            break;
               
        
        extern  void    MtrRdSpecial(u16 addr,u8  len);
        MtrRdSpecial(addr/2,len/2);                             //读入数据(16位计算)
        
        if(MtrCom->RxCtrl.Len == len/2)                         //判断写入是否正确
        {
            memcpy(Com->Rd.dtu.parabuf,(u8 *)&MtrCom->Rd,len);//取出数据
        }else{
            Com->Rd.dtu.paralen = 0;                         //将数据长度置0，表示错误
        }       
            
        break; 
        
    case    CMD_REC_START:                                      //马上启动数据发送
        Ctrl.sRunPara.SysSts.SetBitFlg     = 0;                        //不是参数设置状态
        Com->ConnCtrl.ConnType   = RECORD_SEND_COMM;         //默认为数据发送
        break;
    default:
        Ctrl.sRunPara.SysSts.SetBitFlg     = 0;              //不是参数设置状态
        Com->ConnCtrl.ConnType   = RECORD_SEND_COMM;         //默认为数据发送
        break;
    }
}

extern  void app_iap_deal(void);
/**************************************************************
* Description  : 和DTU通讯，接收部分
* Author       : 2018/5/24 星期三, by redmorningcn
*/
void    app_dtu_rec(StrCOMCtrl *Com)
{
    u8  conntype;                       //数据通许、参数设置、IAP
    u8  enablesend = 0;                 //数据发送标识
    OS_ERR  err;
    
    /**************************************************************
    * Description  : 确定通讯类型。
    如果是程序下载，或者参数读写，则更改为对应的通讯类型。
    * Author       : 2018/5/24 星期四, by redmorningcn
    */
    if(Com->RxCtrl.protocol == CSNC_PROTOCOL)                //csnc异步串口通讯协议
    {
        switch(Com->RxCtrl.FrameCode)
        {
        case IAP_FRAME_CODE:
            Com->ConnCtrl.ConnType = IAP_COMM;               //IAP通讯
            Ctrl.sRunPara.SysSts.SetBitFlg  = 1;            //表示正在设置参数
            Ctrl.sRunPara.SetOutTimes       = 90;
            break;
        case SET_FRAME_CODE:
            Com->ConnCtrl.ConnType = SET_COMM;               //参数读取

            break;
        case RECORD_FRAME_CODE:
            if(Com->ConnCtrl.RecordSendFlg == 1){            //有数据发送，通讯类型不变。
                Com->ConnCtrl.RecordSendFlg = 0;
            }
            //break;
        default:
            Com->ConnCtrl.ConnType       = RECORD_SEND_COMM; //默认为数据发送

            if(Com->RxCtrl.sCsnc.sourceaddr == SET_ADDR)
            {
                Com->ConnCtrl.ConnType   = SET_COMM;         //参数读取
            }
            
            break;
        }
    }
    
    /**************************************************************
    * Description  : 增加gps定位模块
    * Author       : 2018/6/4 星期一, by redmorningcn
    */
    if(Com->RxCtrl.protocol == Q560_PROTOCOL){
        Com->ConnCtrl.ConnType = GPS_COMM;                   //GPS定位模块通讯
    }

    //根据通讯类型处理
    conntype = Com->ConnCtrl.ConnType;
    
    switch(conntype){
        
    case RECORD_SEND_COMM:     
        //接收到发送数据的应答。判断发送应答帧号和接收帧号，相等认为发送成功。
        //判断是否有数据发送。
        enablesend = 0;
        if(Com->ConnCtrl.SendRecordNum == Com->RxCtrl.sCsnc.framnum){
            Ctrl.sRecNumMgr.GrsRead++;
            Com->ConnCtrl.SendRecordNum++;                               //发送记录++
            Ctrl.sRunPara.FramFlg.WrNumMgr = 1;                             //存记录标识有效
            osal_set_event(OS_TASK_ID_STORE,OS_EVT_STORE_FRAM);             //设置存储事件，启动储存任务
            
            if(Ctrl.sRecNumMgr.GrsRead < Ctrl.sRecNumMgr.Current)           //有数据未发送，继续启动发送任务
            {
                enablesend = 1;                                             //再次启动数据发送    
            }
        }
        
        Com->ConnCtrl.RecordSendFlg = 0;
        
        break;  
        
    case SET_COMM:
        
        Ctrl.sRunPara.SysSts.SetBitFlg  = 1;                                //表示正在设置参数
        Ctrl.sRunPara.SetOutTimes       = 60;        
        
        app_operate_para(Com);                                               //参数设置（读取）

        enablesend = 1;                                                     //启动数据发送
        break;
        
        /**************************************************************
        * Description  : 程序升级。应答升级的内容
        * Author       : 2018/5/23 星期三, by redmorningcn
        */
    case IAP_COMM:
        
        app_iap_deal();                 //iap升级处理
        
        Ctrl.sRunPara.SysSts.SetBitFlg  = 1;            //表示正在设置参数
        Ctrl.sRunPara.SetOutTimes       = 90;        
        enablesend = 1;                 //启动数据发送
        break;
        
        
        /**************************************************************
        * Description  : GPS模块通讯,成功接收到数据
        * Author       : 2018/6/4 星期一, by redmorningcn
        */
    case GPS_COMM:
        
        enablesend = 0;   
        Com->ConnCtrl.ConnType       = RECORD_SEND_COMM;                 //默认为数据发送
        break;
    default:
        break;
    }
    
    if( enablesend == 1 ){
        
        if(Com == DtuCom){
            OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.CommEvtFlagGrp,               //通知DTU，可以进行数据发送
                       ( OS_FLAGS      )COMM_EVT_FLAG_DTU_TX,
                       ( OS_OPT        )OS_OPT_POST_FLAG_SET,
                       ( OS_ERR       *)&err
                           );
        }
        
        if(Com == TaxCom){
            OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.CommEvtFlagGrp,               //通知DTU，可以进行数据发送
                       ( OS_FLAGS      )COMM_EVT_FLAG_TAX_TX,
                       ( OS_OPT        )OS_OPT_POST_FLAG_SET,
                       ( OS_ERR       *)&err
                           );
        }
    }
}




