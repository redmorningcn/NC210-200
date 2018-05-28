/**************************************************************
* Description  : 程序下载协议
* Author       : 2018/5/25 星期五, by redmorningcn
*/

/*******************************************************************************
 * INCLUDES
 */
#include  <includes.h>
#include  <IAP_program_mcu.h>
#include  <string.h>

/*******************************************************************************
 * CONSTANTS
 */


/*******************************************************************************
 * TYPEDEFS
 */

     
/**************************************************************
* Description  : app_iap_deal  软件远程或串口升级
* Author       : 2018/5/25 星期五, by redmorningcn
*/
void    app_iap_deal(void){
    stcIAPPara      sLocalIap;                          //存储的iap参数
    stcIAPPara      *sServerIap = (stcIAPPara *)&DtuCom->Rd.dtu.iap;  //服务器下发的iap参数 

    u8      code = DtuCom->Rd.dtu.iap.code;             //IAP指令位
    u8      sta  = DtuCom->Rd.dtu.iap.sta;              //IAP状态位
    u16     crc16;
    u16     idx;                                        //数据标号
    u16     datalen;
    
    switch(code)
    {
        
    case IAP_START:                                     //IAP启动,
        //读已存的iappara信息
        //memcpy((uint8 *)&sLocalIap,(uint8 *)IAP_PARA_START_ADDR,sizeof(stcIAPPara));
        BSP_FlashReadBytes(IAP_PARA_START_ADDR,(uint8 *)&sLocalIap,sizeof(stcIAPPara));
        crc16 = GetCrc16Chk((uint8 *)&sLocalIap,sizeof(sLocalIap)-8);
        
        //对比接收到的iappara信息（版本、大小、已发帧序号、校验、帧号）
        if ( ( sta == 1 )                           ||      // 重新下载
            ( sLocalIap.Idx    == 0xffffffff ) ||          // 存储序号为-1
            ( sLocalIap.Idx    == 0 )          ||          // 存储序号为零
            ( sServerIap->Idx  == 0 )          ||          // 发送下来的序号为零
            ( sLocalIap.Chk    != crc16)       ||          // 校验码不正确
            ( sServerIap->Size != sLocalIap.Size ) ||      // 程序大小不符
                                //( IapServer.Crc32 != IapInfo.Crc32 ) 
            ( sServerIap->SwVer!= sLocalIap.SwVer )        // 版本不符
            ) 
        {
            /**************************************************************
            * Description  : 按重发条件返回
            * Author       : 2018/5/28 星期一, by redmorningcn
            */
            sLocalIap.Addr        = USER_BACK_START_ADDR;       //初始化地址
            sLocalIap.Idx         = 0;
            
            sLocalIap.HwVer       = Ctrl.sProductInfo.HwVer;    //返回装置版本信息
            sLocalIap.SwVer       = Ctrl.sProductInfo.SwVer;
            sLocalIap.Size        = sServerIap->Size;           
            sLocalIap.Crc32       = sServerIap->Crc32;
        }else{
            /**************************************************************
            * Description  : 续传，按存储的地址发送
            * Author       : 2018/5/28 星期一, by redmorningcn
            */
            //sLocalIap.HwVer       = Ctrl.sProductInfo.HwVer;  //续传不更改版本信息
            //sLocalIap.SwVer       = Ctrl.sProductInfo.SwVer;  
            sLocalIap.Addr        = USER_BACK_START_ADDR + sLocalIap.Idx * 128;
        }
        
        //将升级信息返回给服务器，如硬件版本，软件版本，程序大小，当前地址，当前帧号
        memcpy((u8 *)&DtuCom->Rd.dtu.iap,(u8 *)&sLocalIap,sizeof(stcIAPPara)); 
        //未发送改写程序，不存储更改后的sLocalIap值。
        
        break;
    case IAP_DATA:                                      //IAP数据传输应答
        sLocalIap.End   =   0;                          //End=1，程序下载完成，End=0，未完
        if(DtuCom->ConnCtrl.datalen < 4){
            DtuCom->Rd.dtu.iap.sta  = IAP_IDX_ERR;
            break;
        }
        datalen = DtuCom->ConnCtrl.datalen - 4;         //此帧程序长度。减code（1），sta（1），idx（2）
        idx = DtuCom->Rd.dtu.iap.idx;                   //序号
        
        DtuCom->Rd.dtu.iap.sta = IAP_IDX_ERR;           //应答默认正常
        
        /**************************************************************
        * Description  : 程序传输，根据idx协调发送同步。
        接收的idx和想接收的idx相同，接收正确。
        * Author       : 2018/5/28 星期一, by redmorningcn
        */
        if(idx == sLocalIap.Idx)                        //序号相等，继续发送。
        {
            BSP_FlashWriteBytes(sLocalIap.Addr,DtuCom->Rd.dtu.iap.buf,datalen); //将数据写入指定地址
            
            sLocalIap.Addr = sLocalIap.Idx + datalen;
            sLocalIap.Idx++;                                    //序号++，数据往下进行。
            
            u32 size = sLocalIap.Addr - USER_BACK_START_ADDR;   //已下载程序大小
            if(     size > sLocalIap.Size                       //已发送的程序大小已经大于程序大小。
               ||   sLocalIap.Size > USER_BACK_PRO_SIZE)        //程序大于存储区域
            {
                DtuCom->Rd.dtu.iap.sta = IAP_SIZE_ERR;          //程序大小错误 
            }
            //存参数
            crc16 = GetCrc16Chk((uint8 *)&sLocalIap,sizeof(sLocalIap)-8);
            sLocalIap.Chk = crc16;
            BSP_FlashWriteBytes(IAP_PARA_START_ADDR,(uint8 *)&sLocalIap,sizeof(stcIAPPara));

        }else if(idx == sLocalIap.Idx -1){                      //重复帧，不处理（上一帧重复发送）

        }else{                                                  //帧序号错误。
            DtuCom->Rd.dtu.iap.sta  = IAP_IDX_ERR;
        }
        
        DtuCom->Rd.dtu.iap.replyIdx = sLocalIap.Idx;            //应答当前序号。    
        
        break;
    case IAP_END:
        //将显示任务显示暂停2秒钟
        App_DispDelay(2000); 
        
        //结束，完成性判断。（已存大小和程序大小相符）
        if(     sLocalIap.SwVer == DtuCom->Rd.dtu.iap.para.SwVer//软件版本相符
           ||   sLocalIap.Addr  == DtuCom->Rd.dtu.iap.para.Size //已写数据大小等于程序大小
           )    
        {

            sLocalIap.End = 1;              //已经下载完成

            /**************************************************************
            * Description  : 根据程序大小，确认是否下载为boot。
            如果程序小于USER_BOOT_PRO_SIZE，认为是boot程序。写入boot区。
            * Author       : 2018/5/28 星期一, by redmorningcn
            */
            if(sLocalIap.Size +  2048 < USER_BOOT_PRO_SIZE )
            {
                u32 framnum     = (sLocalIap.Size + IAP_DATA_BUF_LEN -1)/ IAP_DATA_BUF_LEN;
                u32 bootaddr    = USER_BOOT_START_ADDR;     //boot初始地址
                u32 backupaddr  = USER_BACK_START_ADDR;
                for(u32 i= 0;i< framnum ;i++){
                    BSP_FlashReadBytes(backupaddr,DtuCom->Rd.dtu.iap.buf,IAP_DATA_BUF_LEN);
                    BSP_FlashWriteBytes(bootaddr,DtuCom->Rd.dtu.iap.buf,IAP_DATA_BUF_LEN);
                    bootaddr    += IAP_DATA_BUF_LEN;
                    backupaddr  += IAP_DATA_BUF_LEN;
                }
                sLocalIap.End = 0;                          //写boot后，清除标识   
                uprintf("boot ok");
            }else{
                uprintf("app ok");
            }
            
            /**************************************************************
            * Description  : 下载完成
            * Author       : 2018/5/28 星期一, by redmorningcn
            */
            sLocalIap.Idx = 0;          //序号清零
            //存参数
            crc16 = GetCrc16Chk((uint8 *)&sLocalIap,sizeof(sLocalIap)-8);
            sLocalIap.Chk = crc16;
            BSP_FlashWriteBytes(IAP_PARA_START_ADDR,(uint8 *)&sLocalIap,sizeof(stcIAPPara));
        }else{
            DtuCom->Rd.dtu.iap.sta  = IAP_END_ERR;      //结束时，信息错误。
            uprintf("iap err.5");
        }
        
        break;
    default:
        DtuCom->Rd.dtu.iap.sta  = IAP_CODE_ERR;         //指令错误
        break;
    }
}


/*******************************************************************************
 * 				end of file                                                    *
 *******************************************************************************/