/*******************************************************************************
* Description  : 数据记录存储及读取操作
* Author       : 2018/5/15 星期二, by redmorningcn
*******************************************************************************/


/*******************************************************************************
* INCLUDES
*/
#include <includes.h>


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
/***********************************************


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


/*******************************************************************************/


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
* 名    称：RoadNum;        		StoreData
* 功    能：RelRoadNum;     		数据存储。根据数据记录号将数据存储到指定
StationNum;    		flash地址;更新记录号等信息。大部分数据记录的
E_StationNum;  		内容在其他任务直接对sCtrl.sRec 中更新。少部分数据内容在该函数中跟新。
* 入口参数：SignalTyp;      	无
* 出口参数：LocoSign;       	无
* 作    者                 ： 	redmornigcn
* 创建日期：LocoWorkState;  	2017-05-15
* 修    改：LocoState;     
* 修改日期：
*******************************************************************************/
u8 SaveOneREcord(u32 *id,u8 *buf,u32 len)
{	
    /***********************************************
    * 描述： 2017/12/17,无名沈：计算数据记录地址
    */
    u32     FlshAddr    = GetRecFlashAddr( *id );		
    u8      ret         = 0;
    u32     size;
    
//    stcFlshRec sRec,gRec;
//    /***********************************************
//    * 描述： 2017/12/17,无名沈：
//    */

//    /***********************************************
//    * 描述： 2017/12/17,无名沈：
//    */  
//    u8     retrys      = 3;
//    uint16_t    CRC_sum1;
//    uint16_t    CRC_sum2;
//    /***********************************************
//    * 描述： 2017/12/17,无名沈：
//    */
//    do {
//        // 数据存储到flash
//        ret = WriteFlshRec(FlshAddr, (stcFlshRec *)&sRec);
//        // 从FLASH中读取出来进行对比
//        ReadFlshRec((stcFlshRec *)&gRec, Ctrl.sRecNumMgr.Current);
//        /**************************************************
//        * 描述： 数据校验
//        */
//        CRC_sum1 = GetCrc16Chk((u8*)&gRec,size-2);
//        CRC_sum2 = gRec.CrcCheck; 
//        if(CRC_sum1 == CRC_sum2) {
//            u32 *p1 = (u32 *)&gRec;
//            u32 *p2 = (u32 *)&sRec;
//            for ( int i = 0; i < 128 / 4; i++) {
//                if ( p1[i] != p2[i] ) {
//                    //Ctrl.sRunPara.SysErr   = (2<<4) + (0<<2) + (1<<0);
//                    goto next;
//                }
//            }
//            //Ctrl.sRunPara.SysErr   &= ~((2<<4) + (0<<2) + (1<<0));
//            // 	保存数据记录号
//            (*id)++;
//            FRAM_StoreRecNumMgr((StrRecNumMgr  *)&Ctrl.sRecNumMgr); //数据记录号加1，并保存
//            ret = DEF_TRUE;
//            break;
//        } else {
//        next:
//            ret = DEF_FALSE;
//        }
//    } while ( --retrys );
//    
//    
//    if ( ret == DEF_FALSE ) {
//        Ctrl.sRunPara.Err.Flag.FlashErr     = TRUE & (~Ctrl.sRunPara.ErrMask.Flag.FlashErr);                  //D14=1：存储器故障
//        do {
//            (*id)++;
//        } while ((*id) * 128 % 4096 != 0);
//        //(*id)++;
//        FRAM_StoreRecNumMgr((StrRecNumMgr  *)&Ctrl.sRecNumMgr);     //数据记录号加1，并保存
//    } else {
//        Ctrl.sRunPara.Err.Flag.FlashErr     = FALSE;                //D14=1：存储器故障
//    }
//    
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
u8	ReadFlshRec(stcFlshRec * sFlshRec,u32 FlshRecNum)
{
	u32		FlshAddr;
	BSP_FlashWaitEvent();
    
	FlshAddr = GetRecFlashAddr( FlshRecNum );
	
	u8 ret = ReadFlsh(FlshAddr,(INT08U *)sFlshRec,sizeof(stcFlshRec));
	
	BSP_FlashSendEvent();
    
	return	ret;
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
*******************************************************************************/
void    BSP_StoreInit(void)
{    


}


/*******************************************************************************
 * 名    称： 首字母大写
 * 功    能： 
 * 入口参数： 无
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2017/12/20
 * 修    改： 
 * 修改日期： 
 * 备    注： 
 *******************************************************************************/
void App_ParaSave(u8 type)
{
//    switch(type) {
//    case 0:
//        /***************************************************
//        * 描述：记录号管理地址：起始地址 = 000
//        */    
//        FRAM_StoreHeadInfo((StrRecHeadInfo  *)&Ctrl.sHeadInfo);
//    case 1:
//        /***************************************************
//        * 描述：记录号管理地址：起始地址 = 016
//        */    
//        FRAM_StoreRecNumMgr((StrRecNumMgr  *)&Ctrl.sRecNumMgr);
//        break;
//    case 2:
//        /***************************************************
//        * 描述：产品信息：起始地址 = 032
//        */
//        FRAM_StoreProductInfo((StrProductInfo  *)&Ctrl.sProductInfo);
//        break;
//    case 3:
//        /***************************************************
//        * 描述：系统运行参数：起始地址 = 064
//        */
//        FRAM_StoreRunPara((stcRunPara  *)&Ctrl.sRunPara); 
//        break;
//    case 4:
//        /***************************************************
//        * 描述：油量计算参数：起始地址 = 096
//        */
//        FRAM_StoreOilPara((StrOilPara  *)&Ctrl.sOilPara); 
//        GetOilPara();
//        break;
//    case 5:
//        /***************************************************
//        * 描述：计算用油箱模型：起始地址 = 128
//        */ 
//        FRAM_StoreCalcModel((stcCalcModel  *)&Ctrl.sCalcModel); 
//        break;
//    case 6:
//        /***************************************************
//        * 描述：数据记录：起始地址 = 536，保存其中一个
//        */
//        FRAM_StoreCurRecord((stcFlshRec  *)&Ctrl.sRecB); 
//        break;
//    case 7:
//        break;
//    case 8:
//        /***************************************************
//        * 描述：测量模块1参数：起始地址 = 792
//        */
//        MBM_FC16_HoldingRegWrN((MODBUS_CH   *)Ctrl.Mtr.pch,
//                               (CPU_INT08U   )1,
//                               (CPU_INT16U   )0,
//                               (CPU_INT16U  *)&Ctrl.MtrPara[0],
//                               (CPU_INT16U   )60);
//        break;
//    case 9:
//        /***************************************************
//        * 描述：测量模块2参数：起始地址 = 1048
//        */
//        MBM_FC16_HoldingRegWrN((MODBUS_CH   *)Ctrl.Mtr.pch,
//                               (CPU_INT08U   )2,
//                               (CPU_INT16U   )0,
//                               (CPU_INT16U  *)&Ctrl.MtrPara[1],
//                               (CPU_INT16U   )60);
//        break;
//    default:
//        FRAM_StoreHeadInfo((StrRecHeadInfo  *)&Ctrl.sHeadInfo);
//        FRAM_StoreRecNumMgr((StrRecNumMgr  *)&Ctrl.sRecNumMgr); 
//        FRAM_StoreProductInfo((StrProductInfo  *)&Ctrl.sProductInfo);
//        FRAM_StoreRunPara((stcRunPara  *)&Ctrl.sRunPara); 
//        FRAM_StoreOilPara((StrOilPara  *)&Ctrl.sOilPara); 
//        FRAM_StoreCalcModel((stcCalcModel  *)&Ctrl.sCalcModel); 
//        FRAM_StoreCurRecord((stcFlshRecNdp02B  *)&Ctrl.sRecB);
//        break;
//    }
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
//    if( task_event & OS_EVT_STORE_TICKS ) {
//        osal_start_timerRl( OS_TASK_ID_STORE,
//                            OS_EVT_STORE_TICKS,
//                            OS_TICKS_PER_SEC * Ctrl.sRunPara.StoreTime);
//        /***************************************************
//        * 描述： 置位保存数据标志位，启动数据保存
//        */
//        App_SaveDataToHistory();
//        
//        return ( task_event ^ OS_EVT_STORE_TICKS );
//    }
    
    /*******************************************************************************
    * Description  : 保存参数（按标识位保存。标识位1，保存对应参数）
    * Author       : 2018/5/15 星期二, by redmorningcn
    *******************************************************************************/
//    if( task_event & OS_EVT_STORE_INIT ) {
//        /***************************************************
//        * 描述： 保存参数
//        */
//        for ( u8 i = 0; i < MAX_STORE_TYPE; i++ ) {
//            //查找当前参数是需要保存
//            if ( SaveType[i] == TRUE ) {
//                //将当前参数标志清零
//                SaveType[i] = FALSE;
//                //保存参数
//                App_ParaSave(i);
//                //稍加延时后再查询是否有其他参数需要保存，如果没有则不会再进入此处
//                osal_start_timerEx( OS_TASK_ID_STORE,
//                                    OS_EVT_STORE_INIT,
//                                    100);
//                break;
//            }
//        }
//        
//        return ( task_event ^ OS_EVT_STORE_INIT );
//    }
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
//    BSP_StoreInit();
//    
//    /***********************************************
//    * 描述： 在看门狗标志组注册本任务的看门狗标志
//    */
//    OSRegWdtFlag(( OS_FLAGS     )WDT_FLAG_STORE );
//    
//    /***********************************************
//    * 描述： 2017/12/3,无名沈：设置存储周期为60秒
//    */
//    osal_start_timerRl( OS_TASK_ID_STORE,
//                        OS_EVT_STORE_TICKS,
//                        OS_TICKS_PER_SEC * 60);
}

/*******************************************************************************
* 				                    end of file                                *
*******************************************************************************/
#endif
