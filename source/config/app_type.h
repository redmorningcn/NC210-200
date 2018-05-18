/*******************************************************************************
* Description  : 定义通用结构体
* Author       : 2018/5/10 星期四, by redmorningcn
*******************************************************************************/
#ifndef  APP_TYPE_H_
#define  APP_TYPE_H_

#include    "stm32f10x_type.h"
#include    <app_com_type.h>
#include    <RecDataTypeDef.h>

#define     SOFT_VERSION                (0101)          /* 软件版本         */
#define     MODBUS_PASSWORD             (6237)          /* modbus通讯密码   */


/*******************************************************************************
* Description  : 控制结构体头部（程序识别）
* Author       : 2018/5/16 星期三, by redmorningcn
*/
typedef struct { 
    u16                 Password;                       // 	2  	用于MODEBUS通讯确认密钥，默认为6237，防止非法改数据
    u16   		        Rsv[7];				            //	2  	软件版本
} StrRecHeadInfo;

__packed
typedef struct {     
 	u16   	            Type;				        //机车类型	2	参见机车类型代码表
 	u16   	            Nbr;				        //机车号		2	
} stcLocoId;

__packed
typedef struct {
    u32                 Head;
    u32                 Tail;
} StrRecord;

//记录号管理:当前记录号，卡已读记录号，无线发送模块记录号
//16 bytes
__packed
typedef struct _StrRecNumMgr {
    u32   		        Current;			        //当前记录号	0~0xffffffff(计算地址)
    u32   		        Record;				        //文件号
    u32   		        GrsRead;			        //无线已读取记录号
    u32   		        IcDump;				        //IC卡转储
}StrRecNumMgr;

//产品信息:  型号+ 编号
//12 bytes
__packed
typedef struct _StrProductInfo {
	u32   		        Type;			            //产品类别
	u32   		        Id;				            //产品编号	16110002
	stcLocoId		    LocoId;		                //机车信息	104(DF4B) - 1000
    u16                 HwVer;                      //硬件版本
    u16                 SwVer;                      //软件版本
    u16                 LoadingTime;                //装车时间
    u16                 LoadingStaff;               //装车人员
    u16                 RepairTime;                 //维修时间
    u16                 RepairStaff;                //维修人员
    u8                  Rsv[12];                    //预留12个
}StrProductInfo;


//运行参数，装置运行相关，数据存储周期，显示参数，恢复出厂设置
//8 bytes
__packed
typedef struct _stcRunPara_					
{
	u16   		        StoreTime;                  // 1    数据记录存储周期
	//
    u8   		        MtrErr[2];                  // 1    传感器错误代码
    u8   		        SysErr;                     // 1    系统错误代码
	u8   		        StoreType ;                 // 1    存储类型
	u8                  AvgCnt;                     // 1    测量平均次数

    union ___u_Framflg {
        struct {
            u16             WrHead      : 1;        // 写sCtrl头
            u16             WrNumMgr    : 1;        // 写记录管理
            u16             WrProduct   : 1;        // 写产品信息
            u16             WrRunPara   : 1;        // 写运行参数
            
            u16             RdHead      : 1;        // 读sCtrl头
            u16             RdNumMgr    : 1;        // 读记录管理
            u16             RdProduct   : 1;        // 读产品信息
            u16             RdRunPara   : 1;        // 读运行参数
            
            u16             Rsv         : 8;        // 预留       
        } ;
        u16                 Flags;
    } FramFlg;
    
    union ___u_init_flag {
        struct  {
            u32                 ConnIcCardFlag  : 1;        // IC卡模块连接成功标志 成功后清零 
            u32                 SetLocoFlag     : 1;        // 机车号设置成功标志 成功后清零
            u32                 SetOilModFlag   : 1;        // 油箱模型设置成功标志 成功后清零
            u32                 SetOilNbrFlag   : 1;        // 模型编号设置成功标志 成功后清零
            
            u32                 SetDensityFlag  : 1;        // 设置密度成功标志 成功后清零
            u32                 SetOffIncFlag   : 1;        // 设置高度加成功标志 成功后清零
            u32                 SetOffDecFlag   : 1;        // 设置高度减成功标志 成功后清零
            u32                 TestDumpFlag    : 1;        // 转储测试成功标志 成功后清零
            
            u32                 ConnMtr1Flag    : 1;        // 测量模块1连接成功标志 成功后清零
            u32                 ConnMtr2Flag    : 1;        // 测量模块2连接成功标志 成功后清零 
            u32                 ConnDtuFlag     : 1;        // 无线模块连接成功标志 成功后清零
            u32                 ConnTaxFlag     : 1;        // TAX通讯成功标志 成功后清零
            
            u32                 TestEngFlag     : 1;        // 柴速测试成功标志 成功后清零
            u32                 TestSpdFlag     : 1;        // 速度测试成功标志 成功后清零
            u32                 ConnPwrFlag     : 1;        // 电量模块连接成功标志 成功后清零 
            u32                 ConnGpsFlag     : 1;        // GPS模块连接成功标志 成功后清零
            
            u32                 SetTimeFlag     : 1;        // 设置时间成功标志 成功后清零 
            u32                 TestIapFlag     : 1;        // IAP升级成功标志 成功后清零
            u32                 RsvFalg         :12;        // 备用
            u32                 OkFlag          : 1;        // 全部初始化成功标志，未初化0零，初始化化后为1
            u32                 InstallFlag     : 1;        // 装车完成标志，未初化0零，初始化化后为1
        } Flag;
        u32                 Flags;
    } InitFlag;
    
    union ___u_err_mask_flag {
        struct  {
            u16   		        ALFlag          : 1;        //低油位报警禁止
            u16   		        AHFlag          : 1;        //高油位报警禁止
            u16   		        Mtr1Flag        : 1;        //测量模块1禁止
            u16   		        Mtr2Flag        : 1;        //测量模块2禁止
            
            u16   		        EngMtrFlag      : 1;        //柴速测量禁止    
            u16   	            SpdMtrFlag      : 1;        //速度测量禁止
            u16   		        ExtCommFlag     : 1;        //扩展通讯禁止
            u16   		        ExtDispFlag     : 1;        //扩展显示禁止
            
            u16   		        PwrCommFlag     : 1;        //电量模块模块禁止
            u16   	            TaxCommFlag     : 1;        //TAX箱测量禁止    
            u16   	            IcCommFlag      : 1;        //IC卡模块禁止
            u16   	            DtuCommFlag     : 1;        //无线模块禁止
            
            u16   	            Rsv             : 4;        //保留
        } Flag;
        u16                 Flags;
    }Mask;                                          // 故障屏蔽
    union ___u_dev_cfg {
        struct  {
            u8   		        PwrEnFlag       : 1;        //
            u8   		        OtrEnFlag       : 1;        //
            u8   		        ExtComEnFlag    : 1;        //
            u8   		        Dsp1EnFlag      : 1;        //
            
            u8   		        Dsp2EnFlag      : 1;        //
            u8   		        TaxEnFlag       : 1;        //    
            u8   		        GpsEnFlag       : 1;        //
            u8   	            Rsv             : 1;        //保留
        } Flag;
        u8                      Flags;                      // 设备配置B0:电量模块;B1:扩展通讯模块；B2:显示模块1；B3显示模块2；B4:GPS模块；B5~B7预留
    } DevCfg; 
    union ___u_system_sts {
        struct  {
            u16                 DispLevel       : 4;        // 4        显示亮度 1~15 
            u16                 Rsv1            : 4;        // 4 
            u16   		        StartFlg        : 1;        // 1    	首次运行
            u16   		        RecClear        : 1;        // 1		数据记录清零，清StrRecNumMgr内容
            u16   		        SysReset        : 1;        // 1    	系统参数重置，清StrRecNumMgr + StrOilPara 	中的内容。
            u16   		        SetBitFlg       : 1;        // 1        设置有效位指示
            u16   	            Rsv2            : 4;        //保留
        } ;
        u16                  Flags;                     // 
    } SysSts; 
    
    union _u_sys_err {
        struct  {
            u8              Sen1OpenErr     : 1;     //D0=1：一端传感器模块故障
            u8              Sen2OpenErr     : 1;     //D1=1：二端传感器模块故障 
            u8              Mtr1CommErr     : 1;     //D2=1：一端测量模块通信故障
            u8              Mtr2CommErr     : 1;     //D3=1：二端测量模块通信故障
            u8              OtrCommErr      : 1;     //D4=1：IC卡模块通讯故障
            u8              PwrCommErr      : 1;     //D5=1：电量模块通讯故障
            u8              ExtCommErr      : 1;     //D6=1：扩展通讯模块通讯故障
            u8              Disp1CommErr    : 1;     //D7=1：一端显示模块通讯故障
            u8              Disp2CommErr    : 1;     //D8=1：二端显示模块通讯故障
            u8              TaxCommErr      : 1;     //D9：TAX通讯故障
            u8              ParaNullErr     : 1;     //D10=1：未设置运行参数
            u8              BoxNullErr      : 1;     //D11=1：未设置油箱模型
            u8              DevNullErr      : 1;     //D12=1：未设置模块
            u8              BoxSetErr       : 1;     //D13=1：油箱模型设置错误
            u8              FlashErr        : 1;     //D14=1：存储器故障
            u8              FramErr         : 1;     //D15=1：铁电故障
            u8              BattLowErr      : 1;     //D16=1：电池电压低
            u8              TimeErr         : 1;     //D17=1：时间故障
            u8              OilLowErr       : 1;     //D18=1：油位低故障
            u8              OilHighErr      : 1;     //D19=1：油位高故障
            u8              RsvErr          : 4;     //D23~20：预留
        } ;
        u8                  Flags[4];
        u32                 Errors;
        //u8   		        MtrErr[2];                  // 1   	    传感器错误代码
        //u8   		        SysErr;                     // 1   	    系统错误代码
        //u8   		        StoreType ;                 // 1   	    存储类型
    } Err;
    union _u_sys_err_mask {
        struct  {
            u8                  Sen1OpenErr     : 1;     //D0=1：一端传感器模块故障
            u8                  Sen2OpenErr     : 1;     //D1=1：二端传感器模块故障 
            u8                  Mtr1CommErr     : 1;     //D2=1：一端测量模块通信故障
            u8                  Mtr2CommErr     : 1;     //D3=1：二端测量模块通信故障
            u8                  OtrCommErr      : 1;     //D4=1：IC卡模块通讯故障
            u8                  PwrCommErr      : 1;     //D5=1：电量模块通讯故障
            u8                  ExtCommErr      : 1;     //D6=1：扩展通讯模块通讯故障
            u8                  Disp1CommErr    : 1;     //D7=1：一端显示模块通讯故障
            u8                  Disp2CommErr    : 1;     //D8=1：二端显示模块通讯故障
            u8                  TaxCommErr      : 1;     //D9：TAX通讯故障
            u8                  ParaNullErr     : 1;     //D10=1：未设置运行参数
            u8                  BoxNullErr      : 1;     //D11=1：未设置油箱模型
            u8                  DevNullErr      : 1;     //D12=1：未设置模块
            u8                  BoxSetErr       : 1;     //D13=1：油箱模型设置错误
            u8                  FlashErr        : 1;     //D14=1：存储器故障
            u8                  FramErr         : 1;     //D15=1：铁电故障
            u8                  BattLowErr      : 1;     //D16=1：电池电压低
            u8                  TimeErr         : 1;     //D17=1：时间故障
            u8                  OilLowErr       : 1;     //D18=1：油位低故障
            u8                  OilHighErr      : 1;     //D19=1：油位高故障
            u8                  RsvErr          : 4;     //D23~20：预留
        } ;
        u8                  Flags[4];
        u32                 Error;
        //u8   		        MtrErr[2];                  // 1   	    传感器错误代码
        //u8   		        SysErr;                     // 1   	    系统错误代码
        //u8   		        StoreType ;                 // 1   	    存储类型
    } ErrMask;
    u8                  Rsv2[7];                    // 预留16个字节
}stcRunPara;



typedef union _Unnctrl_ {
   struct{
        /***************************************************
        * 描述： 系统参数：起始地址 = 000   通讯密码，软件版本，记录号，产品信息
        */ 
        StrRecHeadInfo      sHeadInfo;                      // 16
        
        /***************************************************
        * 描述：记录号管理地址：起始地址 = 016
        */
        StrRecNumMgr        sRecNumMgr;			            // 16
        
        /***************************************************
        * 描述：产品信息：起始地址 = 032
        */
        StrProductInfo	    sProductInfo;			        // 32
        
        /***************************************************
        * 描述：系统运行参数：起始地址 = 064
        */
        stcRunPara	        sRunPara;				        // 32
        
        /*******************************************************************************
        * Description  : 数据记录
        * Author       : 2018/5/16 星期三, by redmorningcn
        */
        stcFlshRec          Rec;
        
        /*******************************************************************************
        * Description  : 串口控制字
        * Author       : 2018/5/16 星期三, by redmorningcn
        */
        StrCOMCtrl          ComCtrl[4];                         //串控制字
    };
   
    u16   buf[512];
        
}Unnctrl;


//变量声明
extern   volatile   Unnctrl     Ctrl;
extern   volatile   StrCOMCtrl  * DtuCom;
extern   volatile   StrCOMCtrl  * MtrCom;
extern   volatile   StrCOMCtrl  * TaxCom;

#endif                                                          /* End of  include.                       */

