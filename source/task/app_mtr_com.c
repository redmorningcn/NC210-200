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

/**************************************************************
* Description  : 通讯状态指示
* Author       : 2018/5/22 星期二, by redmorningcn
*/
void    MtrCommErrjudge(u8   node,u16    sta)
{
    u8  err = 0;    //正常
    
    if(sta == 0)
        err = 1;    //故障
    
    switch(node)
    {
    case 1:
        Ctrl.sRunPara.Err.Speed1CommErr = err;
        break;
    case 2:
        Ctrl.sRunPara.Err.Speed2CommErr = err;
        break;
    case 3:
        Ctrl.sRunPara.Err.Speed3CommErr = err;
        break;
    case 4:
        Ctrl.sRunPara.Err.WorkStaCommErr= err;
        break;
    default:
        break;
    }
}

/**************************************************************
* Description  : 处理板读sys值。读取得值存在MtrCom->Rd中。
* Author       : 2018/5/22 星期二, by redmorningcn
*/
void    MtrRdSys(void)
{
    u8      node = MtrCom->ConnCtrl.MB_Node;    //modbus地址
    u32     addr;
    u32     len;
    u16     sta;
    
    switch(node)
    {
    /**************************************************************
    * Description  : 工况板和速度信号检测板通讯
    * Author       : 2018/5/22 星期二, by redmorningcn
    */
    case 1:
    case 2:
    case 3:
        addr = 0;
        len  = sizeof(strSpeedSys);      
        sta  = MBM_FC03_HoldingRegRd(   MtrCom->pch,
                                        node,
                                        addr,
                                        (u16 *)&MtrCom->Rd,
                                        len);
        break; 
        
    /**************************************************************
    * Description  : 处理板和工况信号检测板通讯
    * Author       : 2018/5/22 星期二, by redmorningcn
    */
    case 4:
        
        break;      
    default:
        break;
    }
    
    MtrCommErrjudge(node,sta);              //故障判断，置runpara中的故障标识位。异常置1，正常置0    
    
    MtrCom->RxCtrl.Len    = sta;            //置数据长度。（长度控制在255以内）
    MtrCom->RxCtrl.RecvFlg = 1;             //置接收完成标识。协调其他任务。
}

/**************************************************************
* Description  : 处理板读检测值。读取得值给Ctrl.Rec。
* Author       : 2018/5/22 星期二, by redmorningcn
*/
void    MtrRdDetect(void)
{
    u8      node = MtrCom->ConnCtrl.MB_Node;        //modbus地址
    u32     addr;
    u32     len;
    u16     sta;
    
    switch(node)
    {
    /**************************************************************
    * Description  : 工况板和速度信号检测板通讯
    * Author       : 2018/5/22 星期二, by redmorningcn
    */
    case 1:
    case 2:
    case 3:
        len  = sizeof(strSpeed) ;   //速度检测板有效参数长度
        addr = sizeof(strSpeedSys) + sizeof(strCoupleChannel) - len;   //有效参数存储在结构体后部

        sta  = MBM_FC03_HoldingRegRd(   MtrCom->pch,
                                        node,
                                        addr,
                                        (u16 *)&MtrCom->Rd,
                                        len);
        
        if( sta == sizeof(strSpeed) )   //数据读取成功
        {
            /******************************************************
            * Description  : 将读取值给Ctrl.Rec
            * Author       : 2018/5/22 星期二, by redmorningcn
            */
            u8  i = 0;
            u32 freq[2];
            for(i = 0;i < 2;i++){
                Ctrl.Rec.speed[node-1].ch[i].fail   = MtrCom->Rd.speed.para[i].fail;
                Ctrl.Rec.speed[node-1].ch[i].hig    = MtrCom->Rd.speed.para[i].Voh;
                Ctrl.Rec.speed[node-1].ch[i].low    = MtrCom->Rd.speed.para[i].Vol;
                Ctrl.Rec.speed[node-1].ch[i].raise  = MtrCom->Rd.speed.para[i].raise;
                Ctrl.Rec.speed[node-1].ch[i].ratio  = MtrCom->Rd.speed.para[i].ratio;
                freq[i]                             = MtrCom->Rd.speed.para[i].freq;
            }
            Ctrl.Rec.speed[node-1].phase            = MtrCom->Rd.speed.ch1_2phase;
            Ctrl.Rec.speed[node-1].vcc              = MtrCom->Rd.speed.vcc_vol;
            if(freq[0] && freq[1])  //频率值有效，计算平均
                Ctrl.Rec.speed[node-1].freq         = ((freq[0] + freq[1])/2);
        }
        
        break; 
    /**************************************************************
    * Description  : 处理板和工况信号检测板通讯
    * Author       : 2018/5/22 星期二, by redmorningcn
    */
    case 4:
        break;      
    default:
        break;
    }
    
    MtrCommErrjudge(node,sta);              //故障判断，置runpara中的故障标识位。异常置1，正常置0    
    
    MtrCom->RxCtrl.Len      = sta;          //置数据长度。（长度控制在255以内）
    MtrCom->RxCtrl.RecvFlg  = 1;            //置接收完成标识。协调其他任务。
}


/**************************************************************
* Description  : 处理板写sys值。写前sys数据存在MtrCom->Wr中。
* Author       : 2018/5/22 星期二, by redmorningcn
*/
void    MtrWrSys(void)
{
    u8      node = MtrCom->ConnCtrl.MB_Node;    //modbus地址
    u32     addr;
    u32     len;
    u16     sta;
    
    switch(node)
    {
    /**************************************************************
    * Description  : 工况板和速度信号检测板通讯
    * Author       : 2018/5/22 星期二, by redmorningcn
    */
    case 1:
    case 2:
    case 3:
        addr = 0;
        len  = sizeof(strSpeedSys);      
   
        sta  = MBM_FC16_HoldingRegWrN ( MtrCom->pch,
                                        node,
                                        addr,
                                        (u16 *)&MtrCom->Wr,
                                        len);
        break; 
        
    /**************************************************************
    * Description  : 处理板和工况信号检测板通讯
    * Author       : 2018/5/22 星期二, by redmorningcn
    */
    case 4:
        
        break;      
    default:
        break;
    }
    
    MtrCommErrjudge(node,sta);              //故障判断，置runpara中的故障标识位。异常置1，正常置0    
    
    MtrCom->ConnCtrl.datalen      = sta;    //置数据长度。（长度控制在255以内）
    MtrCom->ConnCtrl.RecvEndFlg   = 1;      //置接收完成标识。协调其他任务。
}
                                        

/**************************************************************
* Description  : 处理板读Cali值。读取得值存在MtrCom->Rd中。
* Author       : 2018/5/22 星期二, by redmorningcn
*/
void    MtrRdCali(void)
{
    u8      node = MtrCom->ConnCtrl.MB_Node;    //modbus地址
    u32     addr;
    u32     len;
    u16     sta;
    
    switch(node)
    {
    /**************************************************************
    * Description  : 工况板和速度信号检测板通讯
    * Author       : 2018/5/22 星期二, by redmorningcn
    */
    case 1:
    case 2:
    case 3:
        addr = sizeof(strSpeedSys) + sizeof(strCoupleChannel);
        len  = sizeof(strCaliTable); 
        
        sta  = MBM_FC03_HoldingRegRd(   MtrCom->pch,
                                        node,
                                        addr,
                                        (u16 *)&MtrCom->Rd,
                                        len);
        break; 
        
    /**************************************************************
    * Description  : 处理板和工况信号检测板通讯
    * Author       : 2018/5/22 星期二, by redmorningcn
    */
    case 4:
        
        break;      
    default:
        break;
    }
    
    MtrCommErrjudge(node,sta);              //故障判断，置runpara中的故障标识位。异常置1，正常置0    
    
    MtrCom->RxCtrl.Len    = sta;            //置数据长度。（长度控制在255以内）
    MtrCom->RxCtrl.RecvFlg = 1;             //置接收完成标识。协调其他任务。
}                                        


/**************************************************************
* Description  : 处理板写MtrWrCali值。写前MtrWrCali数据存在MtrCom->Wr中。
* Author       : 2018/5/22 星期二, by redmorningcn
*/
void    MtrWrCali(void)
{
    u8      node = MtrCom->ConnCtrl.MB_Node;    //modbus地址
    u32     addr;
    u32     len;
    u16     sta;
    
    switch(node)
    {
    /**************************************************************
    * Description  : 工况板和速度信号检测板通讯
    * Author       : 2018/5/22 星期二, by redmorningcn
    */
    case 1:
    case 2:
    case 3:
        addr = sizeof(strSpeedSys) + sizeof(strCoupleChannel);
        len  = sizeof(strCaliTable);     
   
        sta  = MBM_FC16_HoldingRegWrN ( MtrCom->pch,
                                        node,
                                        addr,
                                        (u16 *)&MtrCom->Wr,
                                        len);
        break; 
        
    /**************************************************************
    * Description  : 处理板和工况信号检测板通讯
    * Author       : 2018/5/22 星期二, by redmorningcn
    */
    case 4:
        
        break;      
    default:
        break;
    }
    
    MtrCommErrjudge(node,sta);              //故障判断，置runpara中的故障标识位。异常置1，正常置0    
    
    MtrCom->ConnCtrl.datalen      = sta;    //置数据长度。（长度控制在255以内）
    MtrCom->ConnCtrl.RecvEndFlg   = 1;      //置接收完成标识。协调其他任务。
}


/**************************************************************
* Description  : 处理板和检测板通讯
* Author       : 2018/5/22 星期二, by redmorningcn
*/
void app_mtr_com(void)
{
    if(MtrCom->ConnCtrl.protocol == MODBUS_PROTOCOL)    //modbus协议
    {
        u8      conntype = MtrCom->ConnCtrl.ConnType;
        switch(conntype)
        {
        case MTR_RD_SYS:
            MtrRdSys();                     //读sys                     
            break;
            
        case MTR_RD_DETECT:
            MtrRdDetect();                  //读检测值
            break;
            
        case MTR_RD_CALI:
            MtrRdCali();                    //读校准系数                    
            break;
            
        case MTR_WR_SYS:
            MtrWrSys();                     //写sys              
            break;
            
        case MTR_WR_CALC:
            break;
            
        default:
            break;
        }
    }
}



