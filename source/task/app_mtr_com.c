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
#include    <string.h>

/**************************************************************
* Description  : ͨѶ״ָ̬ʾ
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
void    MtrCommErrjudge(u8   node,u16    sta)
{
    u8  err = 0;            //����
    
    if(sta == 0)
        err = 1;            //����
    
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
* Description  : ������ָ����ֵַ����ȡ��ֵ����MtrCom->Rd�С�
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
void    MtrRdSpecial(u16 addr,u8  len)
{
    u8      node = MtrCom->ConnCtrl.MB_Node;    //modbus��ַ
    u16     sta;
    
    sta  = MBM_FC03_HoldingRegRd(   MtrCom->pch,
                                    node,
                                    addr,
                                    (u16 *)&MtrCom->Rd,
                                    len);

    MtrCommErrjudge(node,sta);              //�����жϣ���runpara�еĹ��ϱ�ʶλ���쳣��1��������0    
    
    MtrCom->RxCtrl.Len      = sta;            //�����ݳ��ȡ������ȿ�����255���ڣ�
    MtrCom->RxCtrl.RecvFlg  = 1;             //�ý�����ɱ�ʶ��Э����������
}                                        

/**************************************************************
* Description  : �����дָ��λ�á�дǰ���ݴ���MtrCom->Wr�С�
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
void    MtrWrSpecial(u16 addr,u8  len)
{
    u8      node = MtrCom->ConnCtrl.MB_Node;    //modbus��ַ
    u16     sta = 0;
    
    sta  = MBM_FC16_HoldingRegWrN ( MtrCom->pch,
                                    node,
                                    addr,
                                    (u16 *)&MtrCom->Wr,
                                    len);

    MtrCommErrjudge(node,sta);                  //�����жϣ���runpara�еĹ��ϱ�ʶλ���쳣��1��������0    
    
    MtrCom->ConnCtrl.datalen      = sta;        //�����ݳ��ȡ������ȿ�����255���ڣ�
    MtrCom->ConnCtrl.RecvEndFlg   = 1;          //�ý�����ɱ�ʶ��Э����������
}


#define MTR_READ_FLG  2
#define MTR_WRITE_FLG 1
/**************************************************************
* Description  : �����ͼ���ͨѶ
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
void app_mtr_com(void)
{
    u8      i = 0;
    u8      node = MtrCom->ConnCtrl.MB_Node;            //modbus��ַ

    if(MtrCom->ConnCtrl.protocol == MODBUS_PROTOCOL)    //modbusЭ��
    {
        u8      conntype = MtrCom->ConnCtrl.ConnType;
        u16     addr;
        u8      len;
        u16     freq[2];
        switch(conntype)
        {
            
        case MTR_RD_DETECT:
            /**************************************************************
            * Description  : ��ȡ�ٶ��ź�
            * Author       : 2018/5/31 ������, by redmorningcn
            */
            if(node < 4){
                len  = sizeof(strSpeed)/2 ;                                         //�ٶȼ�����Ч��������
                addr = sizeof(strSpeedSys)/2 + sizeof(strCoupleChannel)/2 - len;   //��Ч�����洢�ڽṹ���
                
                MtrRdSpecial(addr,len);
                
                if(MtrCom->RxCtrl.Len == len)
                {   
                    for(i = 0;i < 2;i++){
                        Ctrl.Rec.speed[node-1].ch[i].fail   =  MtrCom->Rd.speed.para[i].fail;
                        Ctrl.Rec.speed[node-1].ch[i].hig    =  MtrCom->Rd.speed.para[i].Voh;
                        Ctrl.Rec.speed[node-1].ch[i].low    =  MtrCom->Rd.speed.para[i].Vol;
                        Ctrl.Rec.speed[node-1].ch[i].raise  =  MtrCom->Rd.speed.para[i].raise;
                        Ctrl.Rec.speed[node-1].ch[i].ratio  =  MtrCom->Rd.speed.para[i].ratio;
                        freq[i]                             =  MtrCom->Rd.speed.para[i].freq;
                    }
                    
                    if((freq[0] == 0) && ((freq[1] == 0))){           //Ƶ��ƫ����5hz���ڡ�
                        Ctrl.Rec.speed[node-1].freq     =   (freq[0] + freq[1])/2;
                        Ctrl.Rec.speed[node-1].sta.freq =   FREQ_CH_OK;
                    }else if(fabs(freq[1]-freq[0]) < 5)
                    {
                        Ctrl.Rec.speed[node-1].freq     =   0;  //Ƶ��Ϊ0
                        Ctrl.Rec.speed[node-1].sta.freq =   FREQ_CH_NONE;
                    }else if((freq[0] == 0)){
                        Ctrl.Rec.speed[node-1].freq     =   freq[1];
                        Ctrl.Rec.speed[node-1].sta.freq =   FREQ_CH1_NONE;
                        
                    }else if((freq[1] == 0)){
                        Ctrl.Rec.speed[node-1].freq     =   freq[0];
                        Ctrl.Rec.speed[node-1].sta.freq =   FREQ_CH2_NONE;
                        
                    }else {
                        
                        Ctrl.Rec.speed[node-1].sta.freq = freq[1];
                        if(freq[0] > freq[1]){
                            Ctrl.Rec.speed[node-1].sta.freq = freq[0];
                        }
                        
                        Ctrl.Rec.speed[node-1].sta.freq     =   FREQ_DIFF;
                    }
                    
                    Ctrl.Rec.speed[node-1].phase    =   MtrCom->Rd.speed.ch1_2phase;
                    Ctrl.Rec.speed[node-1].vcc      =   MtrCom->Rd.speed.vcc_vol;
                }
            }
            
            /**************************************************************
            * Description  : ��ȡ��������ź�
            * Author       : 2018/5/31 ������, by redmorningcn
            */
            if(node == NODE_LOCO){
                len  = sizeof(strlocoVol)/2 ;                                         //����������Ч��������
                addr = sizeof(strLocoSysPara)/2 + sizeof(strLocoRunStatus)/2 - len;   //��Ч�����洢�ڽṹ���
                
                MtrRdSpecial(addr,len);                 
                
                if(MtrCom->RxCtrl.Len == len){
                    memcpy((u8 *)&Ctrl.Rec.Vol,(u8 *)&MtrCom->Rd.loco,sizeof(Ctrl.Rec.Vol));   //ȡ�����ź�
                }
            }
            
            break;
        default:
            break;
        }
    }
}



