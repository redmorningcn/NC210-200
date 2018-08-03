/**************************************************************
* Description  : 处理板和速度检测板及工况检测板通讯
(采用modbus协议，处理板为主机，其他为从。速度检测板地址：1-3，工况检测板地址为：4)
* Author       : 2018/5/21 星期一, by redmorningcn
*/

/*******************************************************************************
* INCLUDES
*/
#include    <includes.h>
#include    <app_mtr_com.h>
#include    <string.h>

/**************************************************************
* Description  : 通讯状态指示
* Author       : 2018/5/22 星期二, by redmorningcn
*/
void    MtrCommErrjudge(u8   node,u16    sta)
{
    u8  err = 0;            //正常
    
    if(sta == 0)
        err = 1;            //故障
    
    switch(node)
    {
    case 1:
        Ctrl.sRunPara.Err.Speed1Comm= err;
        break;
    case 2:
        Ctrl.sRunPara.Err.Speed2Comm= err;
        break;
    case 3:
        Ctrl.sRunPara.Err.Speed3Comm= err;
        break;
    case 4:
        Ctrl.sRunPara.Err.LocoComm  = err;
        break;
    default:
        break;
    }
}
                                        
/**************************************************************
* Description  : 处理板读指定地址值。读取得值存在MtrCom->Rd中。
* Author       : 2018/5/22 星期二, by redmorningcn
*/
void    MtrRdSpecial(u16 addr,u8  len)
{
    u8      node = MtrCom->ConnCtrl.MB_Node;    //modbus地址
    u16     sta;
    
    sta  = MBM_FC03_HoldingRegRd(   MtrCom->pch,
                                    node,
                                    addr,
                                    (u16 *)&MtrCom->Rd,
                                    len);

    MtrCommErrjudge(node,sta);                  //故障判断，置runpara中的故障标识位。异常置1，正常置0    
    
    MtrCom->RxCtrl.Len      = sta;              //置数据长度。（长度控制在255以内）
    MtrCom->RxCtrl.RecvFlg  = 1;                //置接收完成标识。协调其他任务。
}                                        

/**************************************************************
* Description  : 处理板写指定位置。写前数据存在MtrCom->Wr中。
* Author       : 2018/5/22 星期二, by redmorningcn
*/
void    MtrWrSpecial(u16 addr,u8  len)
{
    u8      node = MtrCom->ConnCtrl.MB_Node;    //modbus地址
    u16     sta = 0;
    
    sta  = MBM_FC16_HoldingRegWrN ( MtrCom->pch,
                                    node,
                                    addr,
                                    (u16 *)&MtrCom->Wr,
                                    len);

    MtrCommErrjudge(node,sta);                  //故障判断，置runpara中的故障标识位。异常置1，正常置0    
    
    MtrCom->ConnCtrl.datalen      = sta;        //置数据长度。（长度控制在255以内）
    MtrCom->ConnCtrl.RecvEndFlg   = 1;          //置接收完成标识。协调其他任务。
}


#define MTR_READ_FLG  2
#define MTR_WRITE_FLG 1
/**************************************************************
* Description  : 处理板和检测板通讯
* Author       : 2018/5/22 星期二, by redmorningcn
*/
void app_mtr_com(void)
{
    u8      i = 0;
    u8      node = MtrCom->ConnCtrl.MB_Node;            //modbus地址

    if(MtrCom->ConnCtrl.protocol == MODBUS_PROTOCOL)    //modbus协议
    {
        u8      conntype = MtrCom->ConnCtrl.ConnType;
        u16     addr;
        u8      len;
        u16     freq[2];
        switch(conntype)
        {
            
        case MTR_RD_DETECT:
            /**************************************************************
            * Description  : 读取速度信号
            * Author       : 2018/5/31 星期四, by redmorningcn
            */
            if(node < 4){
                len  = sizeof(strSpeed)/2 ;                                         //速度检测板有效参数长度
                addr = sizeof(strSpeedSys)/2 + sizeof(strCoupleChannel)/2 - len;   //有效参数存储在结构体后部
                
                MtrRdSpecial(addr,len);
                
                if(MtrCom->RxCtrl.Len == len)
                {   
                    for(i = 0;i < 2;i++){
                        Ctrl.Rec.speed[node-1].ch[i].fail   =  MtrCom->Rd.speed.para[i].fail;
                        Ctrl.Rec.speed[node-1].ch[i].hig    =  MtrCom->Rd.speed.para[i].Voh;
                        Ctrl.Rec.speed[node-1].ch[i].low    =  MtrCom->Rd.speed.para[i].Vol;
                        Ctrl.Rec.speed[node-1].ch[i].raise  =  MtrCom->Rd.speed.para[i].raise;
                        Ctrl.Rec.speed[node-1].ch[i].ratio  =  MtrCom->Rd.speed.para[i].ratio;
                        Ctrl.Rec.speed[node-1].sta.flgs[i]  =  (u8)MtrCom->Rd.speed.para[i].status;
                        
                        /**************************************************************
                        * Description  : 加速度判断
                        * Author       : 2018/7/31 星期二, by redmorningcn
                        */
                        s8    acceleration;
                        acceleration    =     (s8)(MtrCom->Rd.speed.para[i].status>>8);
                        
                        Ctrl.Rec.speed[node-1].sta.Err[i].addspeed      = 0;
                        Ctrl.Rec.speed[node-1].sta.Err[i].minusspeed    = 0;

                        if(acceleration > 0){
                            Ctrl.Rec.speed[node-1].sta.Err[i].addspeed      = 1;
                        }else if(acceleration < 0){
                            Ctrl.Rec.speed[node-1].sta.Err[i].minusspeed    = 1;
                        }
                        
                        freq[i]                             =  MtrCom->Rd.speed.para[i].freq;
                    }
                    
                    if(fabs(freq[1]-freq[0]) < 50){                             // 速度信号，速度差值50
                        Ctrl.Rec.speed[node-1].freq     =   (freq[1] + freq[0])/2;
                    }else {
                        if(freq[1] > freq[0]){
                            Ctrl.Rec.speed[node-1].freq     =   freq[1];
                            Ctrl.Rec.speed[node-1].sta.Err[0].FreqLess  = 1;    // 通道0，频率小
                            Ctrl.Rec.speed[node-1].sta.Err[1].FreqMore  = 1;    // 通道1，频率大
                        }else{
                            Ctrl.Rec.speed[node-1].freq     =   freq[0];
                            Ctrl.Rec.speed[node-1].sta.Err[1].FreqLess  = 1;    // 通道0，频率小
                            Ctrl.Rec.speed[node-1].sta.Err[0].FreqMore  = 1;    // 通道1，频率大
                        }
                    }
                    
                    Ctrl.Rec.speed[node-1].phase    =   MtrCom->Rd.speed.ch1_2phase;
                    Ctrl.Rec.speed[node-1].vcc      =   MtrCom->Rd.speed.vcc_vol;
                }
            }
            
            /**************************************************************
            * Description  : 读取工况检测信号
            * Author       : 2018/5/31 星期四, by redmorningcn
            */
            if(node == NODE_LOCO){
                len  = sizeof(strlocoVol)/2 ;                                         //工况检测板有效参数长度
                addr = sizeof(strLocoSysPara)/2 + sizeof(strLocoRunStatus)/2 - len;   //有效参数存储在结构体后部
                
                MtrRdSpecial(addr,len);                 
                
                if(MtrCom->RxCtrl.Len == len){
                    memcpy((u8 *)&Ctrl.Rec.Vol,(u8 *)&MtrCom->Rd.loco,sizeof(Ctrl.Rec.Vol));   //取工况信号
                }
            }
            
            break;
        default:
            break;
        }
    }
}



