/*******************************************************************************
* Description  : 定义及初始化全局变量
* Author       : 2018/5/10 星期四, by redmorningcn
*******************************************************************************/

#include <app_type.h>

/*******************************************************************************
* Description  : 定义全局变量  sCtrl
* Author       : 2018/5/10 星期四, by redmorningcn
*******************************************************************************/
volatile    Unnctrl     Ctrl;              //
    
/*******************************************************************************
* Description  : 无线发送模块控制指针
* Author       : 2018/5/14 星期一, by redmorningcn
*******************************************************************************/
volatile  StrCOMCtrl  * DtuCom;

/*******************************************************************************
* Description  : 和检测板串口控制指针
* Author       : 2018/5/14 星期一, by redmorningcn
*******************************************************************************/
volatile  StrCOMCtrl  * MtrCom;

/*******************************************************************************
* Description  : 和检测板串口控制指针
* Author       : 2018/5/14 星期一, by redmorningcn
*******************************************************************************/
volatile  StrCOMCtrl  * TaxCom;


/*******************************************************************************
 * 名    称： app_init_sctrl
 * 功    能： 
 * 入口参数： 无
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2017/12/27
 * 修    改： 
 * 修改日期： 
 * 备    注： 
 *******************************************************************************/
void app_init_sctrl(void)
{ 

    
    /***********************************************
    * 描述： 2017/12/23,无名沈：串1通信初始化
    */
//    //测量装置1 初始化    
//    Ctrl.Mtr.ConnCtrl[0].ConnFlg            = 1;
//    Ctrl.Mtr.ConnCtrl[0].ErrFlg             = 0;
//    Ctrl.Mtr.ConnCtrl[0].MasterAddr         = 0x80;
//    Ctrl.Mtr.ConnCtrl[0].SlaveAddr          = SLAVE_ADDR_DIP1;
//    Ctrl.Mtr.ConnCtrl[0].SendFlg            = 0;
//    Ctrl.Mtr.ConnCtrl[0].SendFramNum        = 1;
//    Ctrl.Mtr.ConnCtrl[0].TimeOut            = 10;
//    Ctrl.Mtr.ConnCtrl[0].Baud               = 9600;
//    Ctrl.Mtr.ConnCtrl[0].Bits               = UART_DATABIT_8;
//    Ctrl.Mtr.ConnCtrl[0].Parity             = UART_PARITY_NONE;
//    Ctrl.Mtr.ConnCtrl[0].Stops              = UART_STOPBIT_1;
//    

    

    /***********************************************
    * 描述： 2017/12/20,无名沈：读Ctrl
    */

//    WdtReset();

    /***********************************************
    * 描述： 2017/12/27,无名沈： 如果为第一次运行
    */
    CPU_INT08U   retrys  = 3;
    CPU_INT08U   ret     = 0;

    //通讯密码
    Ctrl.sHeadInfo.Password = 6237;

    //软件版本YYMMX
    Ctrl.sProductInfo.SwVer         = SOFT_VERSION;
    /***********************************************
    * 描述： 2017/12/27,无名沈： 读产品编号
    */
    //开始标示 
    Ctrl.sRunPara.SysSts.StartFlg              = 1;
    
    if ( ( Ctrl.sRunPara.StoreTime < 5 ) || 
         ( Ctrl.sRunPara.StoreTime > 10*60 ) ) {
        Ctrl.sRunPara.StoreTime    = 60;
    }
    
    /***********************************************
    * 描述： 2017/12/20,无名沈：
    */
    Ctrl.sRunPara.StoreType         = 0x00;
    //清零故障代码
    Ctrl.sRunPara.Err.Error         = 0;
}
