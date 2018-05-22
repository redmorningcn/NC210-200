/**************************************************************
* Description  : �������ٶȼ��弰��������ͨѶ
(����modbusЭ�飬�����Ϊ����������Ϊ�ӡ��ٶȼ����ַ��1-3�����������ַΪ��4)
* Author       : 2018/5/21 ����һ, by redmorningcn
*/

/*******************************************************************************
* INCLUDES
*/
#include    <includes.h>
#include    <app_mtr_com.h>

/**************************************************************
* Description  : ͨѶ״ָ̬ʾ
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
void    MtrCommErrjudge(u8   node,u16    sta)
{
    u8  err = 0;    //����
    
    if(sta == 0)
        err = 1;    //����
    
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
* Description  : ������sysֵ����ȡ��ֵ����MtrCom->Rd�С�
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
void    MtrRdSys(void)
{
    u8      node = MtrCom->ConnCtrl.MB_Node;    //modbus��ַ
    u32     addr;
    u32     len;
    u16     sta;
    
    switch(node)
    {
    /**************************************************************
    * Description  : ��������ٶ��źż���ͨѶ
    * Author       : 2018/5/22 ���ڶ�, by redmorningcn
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
    * Description  : �����͹����źż���ͨѶ
    * Author       : 2018/5/22 ���ڶ�, by redmorningcn
    */
    case 4:
        
        break;      
    default:
        break;
    }
    
    MtrCommErrjudge(node,sta);              //�����жϣ���runpara�еĹ��ϱ�ʶλ���쳣��1��������0    
    
    MtrCom->RxCtrl.Len    = sta;            //�����ݳ��ȡ������ȿ�����255���ڣ�
    MtrCom->RxCtrl.RecvFlg = 1;             //�ý�����ɱ�ʶ��Э����������
}

/**************************************************************
* Description  : ���������ֵ����ȡ��ֵ��Ctrl.Rec��
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
void    MtrRdDetect(void)
{
    u8      node = MtrCom->ConnCtrl.MB_Node;        //modbus��ַ
    u32     addr;
    u32     len;
    u16     sta;
    
    switch(node)
    {
    /**************************************************************
    * Description  : ��������ٶ��źż���ͨѶ
    * Author       : 2018/5/22 ���ڶ�, by redmorningcn
    */
    case 1:
    case 2:
    case 3:
        len  = sizeof(strSpeed) ;   //�ٶȼ�����Ч��������
        addr = sizeof(strSpeedSys) + sizeof(strCoupleChannel) - len;   //��Ч�����洢�ڽṹ���

        sta  = MBM_FC03_HoldingRegRd(   MtrCom->pch,
                                        node,
                                        addr,
                                        (u16 *)&MtrCom->Rd,
                                        len);
        
        if( sta == sizeof(strSpeed) )   //���ݶ�ȡ�ɹ�
        {
            /******************************************************
            * Description  : ����ȡֵ��Ctrl.Rec
            * Author       : 2018/5/22 ���ڶ�, by redmorningcn
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
            if(freq[0] && freq[1])  //Ƶ��ֵ��Ч������ƽ��
                Ctrl.Rec.speed[node-1].freq         = ((freq[0] + freq[1])/2);
        }
        
        break; 
    /**************************************************************
    * Description  : �����͹����źż���ͨѶ
    * Author       : 2018/5/22 ���ڶ�, by redmorningcn
    */
    case 4:
        break;      
    default:
        break;
    }
    
    MtrCommErrjudge(node,sta);              //�����жϣ���runpara�еĹ��ϱ�ʶλ���쳣��1��������0    
    
    MtrCom->RxCtrl.Len      = sta;          //�����ݳ��ȡ������ȿ�����255���ڣ�
    MtrCom->RxCtrl.RecvFlg  = 1;            //�ý�����ɱ�ʶ��Э����������
}


/**************************************************************
* Description  : �����дsysֵ��дǰsys���ݴ���MtrCom->Wr�С�
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
void    MtrWrSys(void)
{
    u8      node = MtrCom->ConnCtrl.MB_Node;    //modbus��ַ
    u32     addr;
    u32     len;
    u16     sta;
    
    switch(node)
    {
    /**************************************************************
    * Description  : ��������ٶ��źż���ͨѶ
    * Author       : 2018/5/22 ���ڶ�, by redmorningcn
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
    * Description  : �����͹����źż���ͨѶ
    * Author       : 2018/5/22 ���ڶ�, by redmorningcn
    */
    case 4:
        
        break;      
    default:
        break;
    }
    
    MtrCommErrjudge(node,sta);              //�����жϣ���runpara�еĹ��ϱ�ʶλ���쳣��1��������0    
    
    MtrCom->ConnCtrl.datalen      = sta;    //�����ݳ��ȡ������ȿ�����255���ڣ�
    MtrCom->ConnCtrl.RecvEndFlg   = 1;      //�ý�����ɱ�ʶ��Э����������
}
                                        

/**************************************************************
* Description  : ������Caliֵ����ȡ��ֵ����MtrCom->Rd�С�
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
void    MtrRdCali(void)
{
    u8      node = MtrCom->ConnCtrl.MB_Node;    //modbus��ַ
    u32     addr;
    u32     len;
    u16     sta;
    
    switch(node)
    {
    /**************************************************************
    * Description  : ��������ٶ��źż���ͨѶ
    * Author       : 2018/5/22 ���ڶ�, by redmorningcn
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
    * Description  : �����͹����źż���ͨѶ
    * Author       : 2018/5/22 ���ڶ�, by redmorningcn
    */
    case 4:
        
        break;      
    default:
        break;
    }
    
    MtrCommErrjudge(node,sta);              //�����жϣ���runpara�еĹ��ϱ�ʶλ���쳣��1��������0    
    
    MtrCom->RxCtrl.Len    = sta;            //�����ݳ��ȡ������ȿ�����255���ڣ�
    MtrCom->RxCtrl.RecvFlg = 1;             //�ý�����ɱ�ʶ��Э����������
}                                        


/**************************************************************
* Description  : �����дMtrWrCaliֵ��дǰMtrWrCali���ݴ���MtrCom->Wr�С�
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
void    MtrWrCali(void)
{
    u8      node = MtrCom->ConnCtrl.MB_Node;    //modbus��ַ
    u32     addr;
    u32     len;
    u16     sta;
    
    switch(node)
    {
    /**************************************************************
    * Description  : ��������ٶ��źż���ͨѶ
    * Author       : 2018/5/22 ���ڶ�, by redmorningcn
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
    * Description  : �����͹����źż���ͨѶ
    * Author       : 2018/5/22 ���ڶ�, by redmorningcn
    */
    case 4:
        
        break;      
    default:
        break;
    }
    
    MtrCommErrjudge(node,sta);              //�����жϣ���runpara�еĹ��ϱ�ʶλ���쳣��1��������0    
    
    MtrCom->ConnCtrl.datalen      = sta;    //�����ݳ��ȡ������ȿ�����255���ڣ�
    MtrCom->ConnCtrl.RecvEndFlg   = 1;      //�ý�����ɱ�ʶ��Э����������
}


/**************************************************************
* Description  : �����ͼ���ͨѶ
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
void app_mtr_com(void)
{
    if(MtrCom->ConnCtrl.protocol == MODBUS_PROTOCOL)    //modbusЭ��
    {
        u8      conntype = MtrCom->ConnCtrl.ConnType;
        switch(conntype)
        {
        case MTR_RD_SYS:
            MtrRdSys();                     //��sys                     
            break;
            
        case MTR_RD_DETECT:
            MtrRdDetect();                  //�����ֵ
            break;
            
        case MTR_RD_CALI:
            MtrRdCali();                    //��У׼ϵ��                    
            break;
            
        case MTR_WR_SYS:
            MtrWrSys();                     //дsys              
            break;
            
        case MTR_WR_CALC:
            break;
            
        default:
            break;
        }
    }
}



