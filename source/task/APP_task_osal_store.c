/*******************************************************************************
* Description  : 数据记录存储及读取操作
* Author       : 2018/5/15 星期二, by redmorningcn
*******************************************************************************/


/*******************************************************************************
* INCLUDES
*/
#include <includes.h>
#include <string.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_store__c = "$Id: $";
#endif

#define APP_TASK_STORE_EN     DEF_ENABLED
#if APP_TASK_STORE_EN == DEF_ENABLED
/*******************************************************************************
* CONSTANTS
*/

/*******************************************************************************
* MACROS
*/
#define	START_EVT		        31
#define MAX_STORE_TYPE          10

/*******************************************************************************
* TYPEDEFS
*/

/*******************************************************************************
* LOCAL VARIABLES
*/

/*******************************************************************************
* GLOBAL VARIABLES
*/

/*******************************************************************************
* LOCAL FUNCTIONS
*/


/*******************************************************************************
* GLOBAL FUNCTIONS
*/

/*******************************************************************************
* EXTERN VARIABLES
*/


#if (UCOS_EN     == DEF_ENABLED)

#if OS_VERSION > 30000U
static  OS_SEM			Bsp_FlashSem;    	//信号量
#else
static  OS_EVENT      *Bsp_FlashSem;       //信号量
#endif

/*******************************************************************************
* 名    称： BSP_FlashWaitEvent
* 功    能： 等待信号量
* 入口参数： 无
* 出口参数： 0（操作有误），1（操作成功）
* 作    者： redmorningcn
* 创建日期： 2017-05-15
* 修    改：
* 修改日期：
* 备    注： 仅在使用UCOS操作系统时使用
*******************************************************************************/
u8 BSP_FlashOsInit(void)
{    
    /***********************************************
    * 描述： OS接口
    */
#if (UCOS_EN     == DEF_ENABLED)
#if OS_VERSION > 30000U
    BSP_OS_SemCreate(&Bsp_FlashSem,1, "Bsp_FlashSem");      // 创建信号量
#else
    Bsp_FramSem     = OSSemCreate(1);                       // 创建信号量
#endif
#endif 
    return TRUE;
}

/*******************************************************************************
* 名    称： BSP_FlashWaitEvent
* 功    能： 等待信号量
* 入口参数： 无
* 出口参数： 0（操作有误），1（操作成功）
* 作    者： redmorningcn
* 创建日期： 2017-05-15
* 修    改：
* 修改日期：
* 备    注： 仅在使用UCOS操作系统时使用
*******************************************************************************/
static u8 BSP_FlashWaitEvent(void)
{
    /***********************************************
    * 描述： OS接口
    */
    return BSP_OS_SemWait(&Bsp_FlashSem,0); 
}
/*******************************************************************************
* 名    称： FRAM_SendEvent
* 功    能： 释放信号量
* 入口参数： 无
* 出口参数： 无
* 作    者： redmorningcn
* 创建日期： 2017-05-15
* 修    改：
* 修改日期：
* 备    注： 仅在使用UCOS操作系统时使用
*******************************************************************************/
static void BSP_FlashSendEvent(void)
{
    BSP_OS_SemPost(&Bsp_FlashSem);                        	// 发送信号量
}
#endif
/*******************************************************************************/

u8	    WriteFlshRec(u32 FlshAddr,stcFlshRec *sFlshRec);
void	ReadFlshRec(stcFlshRec * sFlshRec,u32 FlshRecNum);

/*******************************************************************************
* 名    称： GetRecFlashAddr
* 功    能： 取数据记录地址。
* 入口参数： 存储记录号
* 出口参数： flash存储地址
* 作    者： redmornigcn
* 创建日期： 2017-05-15
* 修    改：
* 修改日期：
*******************************************************************************/
static u32  GetRecFlashAddr(u32 FlshRecNum)
{				
    return  (u32)(((FlshRecNum * sizeof(stcFlshRec)) % FLSH_MAX_SIZE)); 			
}

/*******************************************************************************
* Description  : 数据记录赋值（部分数据在各任务中赋值）
* Author       : 2018/5/17 星期四, by redmorningcn
*/
static  void    FmtRecord(void)
{
    Ctrl.Rec.CmdTpye    = 01;
    Ctrl.Rec.EvtType    = 0;
    
    if(Ctrl.sRunPara.SysSts.StartFlg == 1){
        Ctrl.Rec.EvtType =  START_EVT;                              //开机事件
    }
    
    time_s t_tm  = TIME_GetCalendarTime();                          //取记录时间
    
    Ctrl.Rec.Year   = t_tm.tm_year-2000;
    Ctrl.Rec.Mon    = t_tm.tm_mon;
    Ctrl.Rec.Day    = t_tm.tm_mday;	
    Ctrl.Rec.Hour   = t_tm.tm_hour;	
    Ctrl.Rec.Min    = t_tm.tm_min;	
    Ctrl.Rec.Sec    = t_tm.tm_sec;   
    
    memset((u8 *)Ctrl.Rec.rsvbyte,0,sizeof(Ctrl.Rec.rsvbyte));    //预留数值置零
}

/*******************************************************************************
* 名    称：RoadNum;        		StoreData
* 功    能：RelRoadNum;     		数据存储。根据数据记录号将数据存储到指定
StationNum;    		flash地址;更新记录号等信息。大部分数据记录的
E_StationNum;  		内容在其他任务直接对sCtrl.sRec 中更新。少部分数据内容在该函数中跟新。
* 入口参数：SignalTyp;      	无
* 出口参数：LocoSign;       	无
* 作    者        ： 	redmornigcn
* 创建日期：LocoWorkState;  	2017-05-15
* 修    改：LocoState;     
* 修改日期：
*******************************************************************************/
u8 App_SaveRecord(void)
{	
    u32         addr ;	
    u16         CRC_sum1;
    u16         CRC_sum2;
    u16         retrys;
    u8          ret;

    stcFlshRec  sRectmp;        //数据记录，临时存储

    /*******************************************************************************
    * Description  : 设置记录值
    * Author       : 2018/5/17 星期四, by redmorningcn
    */
    FmtRecord();
    
    retrys = 3;                                                             //错误允许错误次数
    
    do {
        /*******************************************************************************
        * Description  : 计算flash地址；循环存储
        * Author       : 2018/5/16 星期三, by redmorningcn
        */
        addr = GetRecFlashAddr(Ctrl.sRecNumMgr.Current);

        memcpy((u8 *)&sRectmp,(u8 *)&Ctrl.Rec,sizeof(Ctrl.Rec));        //数据拷贝，全局数据变更
        sRectmp.CrcCheck = GetCrc16Chk((u8*)&sRectmp,sizeof(stcFlshRec)-2);//计算校验
        // 数据存储到flash
        ret = WriteFlshRec(addr, (stcFlshRec *)&sRectmp);
        // 从FLASH中读取出来进行对比
        ReadFlshRec((stcFlshRec *)&sRectmp, Ctrl.sRecNumMgr.Current);
        /**************************************************
        * 描述： 数据校验
        */
        CRC_sum1 = GetCrc16Chk((u8*)&sRectmp,sizeof(stcFlshRec)-2);
        CRC_sum2 = sRectmp.CrcCheck; 
        if(CRC_sum1 == CRC_sum2) {                                      //数据记录正确,记录编号增加
            Ctrl.sRecNumMgr.Current++;          
            Ctrl.sRecNumMgr.Record++;
            Ctrl.Rec.RecordId = Ctrl.sRecNumMgr.Current;                
            break;
        } 
        else
        {
            Ctrl.sRecNumMgr.Current++;                                  //记录跳转  
            if( retrys == 2 )                                       
            {
                if(Ctrl.sRecNumMgr.Current % (4096/128) != 0)           //翻转一页，再试一次。
                    Ctrl.sRecNumMgr.Current++; 
            }
        }
    } while ( --retrys );

    if ( ret == DEF_FALSE || retrys == 0) {                             //报flash错误
        Ctrl.sRunPara.Err.FlashErr     = FALSE;                         //D14=1：存储器故障
    }

    Ctrl.sRunPara.FramFlg.WrNumMgr = 1;         
    osal_set_event( OS_TASK_ID_STORE, OS_EVT_STORE_FRAM);               //通知存参数FRAM

    return ret;
}

/*******************************************************************************
* 名    称： ReadFlshRec
* 功    能： 根据记录号，取数据记录
* 入口参数： 无
* 出口参数： 无
* 作    者： redmornigcn
* 创建日期： 2017-05-15
* 修    改：
* 修改日期：
*******************************************************************************/
void	ReadFlshRec(stcFlshRec * sFlshRec,u32 FlshRecNum)
{
	u32		FlshAddr;
	BSP_FlashWaitEvent();
    
	FlshAddr = GetRecFlashAddr( FlshRecNum );
	
	ReadFlsh(FlshAddr,( INT08U * )sFlshRec,sizeof( stcFlshRec ));
	
	BSP_FlashSendEvent();
    
	return	;
}

/*******************************************************************************
* 名    称： ReadFlshRec
* 功    能： 根据记录号，取数据记录
* 入口参数： 无
* 出口参数： 无
* 作    者： redmornigcn
* 创建日期： 2017-05-15
* 修    改：
* 修改日期：
*******************************************************************************/
u8	WriteFlshRec(u32 FlshAddr,stcFlshRec *sFlshRec)
{
	BSP_FlashWaitEvent();
    
	u8 ret = WriteFlsh(FlshAddr,(INT08U *)sFlshRec, sizeof(stcFlshRec));
	
	BSP_FlashSendEvent();
    
	return	ret;
}

/*******************************************************************************
* 名    称：  TaskInitStore
* 功    能：  任务初始化
* 入口参数： 	无
* 出口参数： 	无
* 作    者： 	redmornigcn
* 创建日期： 	2017-05-15
* 修    改：
* 修改日期：
*/
void    BSP_StoreInit(void)
{    
    SPI_FLASH_Init();                       //初始化mx25 spi接口
}

/*******************************************************************************
* Description  : 保存参数
* Author       : 2018/5/16 星期三, by redmorningcn
*/
void App_FramPara(void)
{
    int     add;
    
    if(Ctrl.sRunPara.FramFlg.Flags)                                        
    {
        /***********************************************************************
        * Description  : 存HEAD
        * Author       : 2018/5/16 星期三, by redmorningcn
        */
        if(Ctrl.sRunPara.FramFlg.WrHead == 1)                               
        {
            Ctrl.sRunPara.FramFlg.WrHead = 0;
            add = (int)&Ctrl.sHeadInfo - (int)&Ctrl;                                
            
            WriteFM24CL64(add,(u8 *)&Ctrl.sHeadInfo,sizeof(Ctrl.sHeadInfo));  
        }
        /***********************************************************************
        * Description  : 读HEAD
        * Author       : 2018/5/16 星期三, by redmorningcn
        */
        if(Ctrl.sRunPara.FramFlg.RdHead == 1)                               
        {
            Ctrl.sRunPara.FramFlg.RdHead = 0;
            add = (int)&Ctrl.sHeadInfo - (int)&Ctrl;                                
            
            ReadFM24CL64(add,(u8 *)&Ctrl.sHeadInfo,sizeof(Ctrl.sHeadInfo));  
        }
        
        /***********************************************************************
        * Description  : 存NumMgr
        * Author       : 2018/5/16 星期三, by redmorningcn
        */
        if(Ctrl.sRunPara.FramFlg.WrNumMgr == 1)
        {
            Ctrl.sRunPara.FramFlg.WrNumMgr = 0;
            add = (int)&Ctrl.sRecNumMgr - (int)&Ctrl;
            
            WriteFM24CL64(add,(u8 *)&Ctrl.sRecNumMgr,sizeof(Ctrl.sRecNumMgr));
        }
        /***********************************************************************
        * Description  : 读NumMgr
        * Author       : 2018/5/16 星期三, by redmorningcn
        */
        if(Ctrl.sRunPara.FramFlg.RdNumMgr == 1)
        {
            Ctrl.sRunPara.FramFlg.RdNumMgr = 0;
            add = (int)&Ctrl.sRecNumMgr - (int)&Ctrl;
            
            ReadFM24CL64(add,(u8 *)&Ctrl.sRecNumMgr,sizeof(Ctrl.sRecNumMgr));
        }
        
        /***********************************************************************
        * Description  : 存产品信息
        * Author       : 2018/5/16 星期三, by redmorningcn
        */
        if(Ctrl.sRunPara.FramFlg.WrProduct == 1)
        {
            Ctrl.sRunPara.FramFlg.WrProduct = 0;
            add = (int)&Ctrl.sProductInfo - (int)&Ctrl;
            
            WriteFM24CL64(add,(u8 *)&Ctrl.sProductInfo,sizeof(Ctrl.sProductInfo));
        }
        
        /***********************************************************************
        * Description  : 读产品信息
        * Author       : 2018/5/16 星期三, by redmorningcn
        */
        if(Ctrl.sRunPara.FramFlg.RdProduct == 1)
        {
            Ctrl.sRunPara.FramFlg.RdProduct = 0;
            add = (int)&Ctrl.sProductInfo - (int)&Ctrl;
            
            ReadFM24CL64(add,(u8 *)&Ctrl.sProductInfo,sizeof(Ctrl.sProductInfo));
        }        
        
        /***********************************************************************
        * Description  : 存运行信息
        * Author       : 2018/5/16 星期三, by redmorningcn
        */
        if(Ctrl.sRunPara.FramFlg.WrRunPara == 1)
        {
            Ctrl.sRunPara.FramFlg.WrRunPara = 0;
            add = (int)&Ctrl.sProductInfo - (int)&Ctrl;
            
            WriteFM24CL64(add,(u8 *)&Ctrl.sRunPara,sizeof(Ctrl.sRunPara));
        } 
        
        /***********************************************************************
        * Description  : 读运行信息
        * Author       : 2018/5/16 星期三, by redmorningcn
        */
        if(Ctrl.sRunPara.FramFlg.RdRunPara == 1)
        {
            Ctrl.sRunPara.FramFlg.RdRunPara = 0;
            add = (int)&Ctrl.sProductInfo - (int)&Ctrl;
            
            ReadFM24CL64(add,(u8 *)&Ctrl.sRunPara,sizeof(Ctrl.sRunPara));
        }    
    }
        
    Ctrl.sRunPara.FramFlg.Flags = 0;               //清标识
}

/*******************************************************************************
* 名    称： 		AppTaskStore
* 功    能： 		控制任务
* 入口参数： 	p_arg - 由任务创建函数传入
* 出口参数： 	无
* 作    者： 	redmorningcn.
* 创建日期： 	2017-05-15
* 修    改：
* 修改日期：
*******************************************************************************/
osalEvt  TaskStoreEvtProcess(osalTid task_id, osalEvt task_event)
{
    /***********************************************
    * 描述： 本任务看门狗标志置位
    */
    //OSSetWdtFlag(( OS_FLAGS     ) WDT_FLAG_STORE);
    
    /*******************************************************************************
    * Description  : 保存数据记录（按Ctrl.sRunPara.StoreTime时间间隔进行数据保存）
    * Author       : 2018/5/15 星期二, by redmorningcn
    *******************************************************************************/
    if( task_event & OS_EVT_STORE_TICKS ) {
        osal_start_timerRl( OS_TASK_ID_STORE,
                            OS_EVT_STORE_TICKS,
                            OS_TICKS_PER_SEC * Ctrl.sRunPara.StoreTime);
        /***************************************************
        * 描述： 置位保存数据标志位，启动数据保存FLASH
        */
        App_SaveRecord();                                       //保存数据记录
                                                
        OS_ERR      err;
        OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.CommEvtFlagGrp,   //通知DTU，可以进行数据发送
                   ( OS_FLAGS      )COMM_EVT_FLAG_DTU_TX,
                   ( OS_OPT        )OS_OPT_POST_FLAG_SET,
                   ( OS_ERR       *)&err);
        
        return ( task_event ^ OS_EVT_STORE_TICKS );
    }
    
    /*******************************************************************************
    * Description  : 保存参数（按标识位保存。标识位1，保存对应参数）
    * Author       : 2018/5/15 星期二, by redmorningcn
    *******************************************************************************/
    if( task_event & OS_EVT_STORE_FRAM ) {
        /***************************************************
        * 描述： 铁电参数存储或读取
        */
        App_FramPara();
        
        return ( task_event ^ OS_EVT_STORE_FRAM );
    }
    return 0;
}

/*******************************************************************************
* 名    称： TaskInitStore
* 功    能： 任务初始化
* 入口参数： 无
* 出口参数： 无
* 作    者： redmornigcn
* 创建日期： 2017-05-15
* 修    改：
* 修改日期：
*******************************************************************************/
void TaskInitStore(void)
{    
    /***********************************************
    * 描述： 初始化Flash底层相关函数
    */
    BSP_StoreInit();
    
//    /***********************************************
//    * 描述： 在看门狗标志组注册本任务的看门狗标志
//    */
//    OSRegWdtFlag(( OS_FLAGS     )WDT_FLAG_STORE );
//    
    /***********************************************
    * 描述： 2017/12/3,无名沈：设置存储周期为60秒
    */
    osal_start_timerRl( OS_TASK_ID_STORE,
                        OS_EVT_STORE_TICKS,
                        //OS_TICKS_PER_SEC * 60);
                        OS_TICKS_PER_SEC * 6);
}


/**************************************************************
* Description  : 根据指定记录号，读取数据记录
如果数据记录读取错误，重新赋值机车信息，数据记录信息，故障状态信息及校验。
* Author       : 2018/5/23 星期三, by redmorningcn
*/
void    app_ReadOneRecord(stcFlshRec *pRec,u32 num)
{
    u16         crc1;
    u16         crc2;
    u8          retrys = 2;
    
    do {
        ReadFlshRec(pRec, num); //读取指定记录号的数据
        
        crc1 = GetCrc16Chk((u8 *)pRec,sizeof(stcFlshRec)-2); 
        crc2 = pRec->CrcCheck;
        
        if(crc1 == crc2) {      //如果校验通过，数据读取完成，退出
            return;
        }
    } while ( --retrys );
    
    /**************************************************
    * 描述： 重新赋值机车信息，数据记录信息，故障状态信息及校验
    */ 
    
    pRec->RecordId    = num;         //记录号
    pRec->LocoType    = Ctrl.sProductInfo.LocoId.Type  ;//机车型号       2  
    pRec->LocoNbr     = Ctrl.sProductInfo.LocoId.Nbr   ;//机 车 号       2 
    //pRec->Err.= 1;        //flash故障
    crc1              = GetCrc16Chk((u8 *)pRec,sizeof(stcFlshRec)-2);
    pRec->CrcCheck    = crc1;
    
}

/*******************************************************************************
* 				                    end of file                                *
*******************************************************************************/
#endif
