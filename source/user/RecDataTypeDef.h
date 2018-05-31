/*******************************************************************************
* Description  :  ����洢��¼�����ݽṹ
* Author       : 2018/5/15 ���ڶ�, by redmorningcn
*******************************************************************************/

#ifndef  _RecDataTypeDef_h_
#define  _RecDataTypeDef_h_

#include <includes.h>

/*******************************************************************************
* Description  : �ٶ�ͨ���洢���ݽṹ
* Author       : 2018/5/15 ���ڶ�, by redmorningcn
*******************************************************************************/
typedef struct _stcSpeed
{
    
    union{
        struct{
            u16 freq    :3 ;    //00��Ƶ��Ϊ0��01��ͨ��1Ƶ��Ϊ0��02��ͨ��2Ƶ��Ϊ0��03��Ƶ��������04��Ƶ��ƫ���
            u16 rec     :13;    //Ԥ��
        };
        u16 flgs;
    }sta;
    u16     phase;
    u16     vcc;
    u16     freq;
    struct _stcCH
    {
        u16 raise;
        u16 fail;
        u16 ratio;    
        u16 hig;
        u16 low;    
    }ch[2];
}sSpeed;


/*******************************************************************************
* Description  : ����ռ��flash�洢�����ݽṹ��128�ֽڣ�
* Author       : 2018/5/15 ���ڶ�, by redmorningcn
*******************************************************************************/
__packed
typedef  struct _stcFlshRec                 //����ռ��¼���ݽṹ
{
    u8   				CmdTpye		        ;//��������       1
    u8   				EvtType             ;//�¼�����       1
    u16   				LocoType            ;//�����ͺ�       2
    u16   				LocoNbr             ;//�� �� ��       2
    u32   				RecordId            ;//��¼��ˮ��     4
    
    struct {                                //��¼ʱ��        4
        u32      Sec                 : 6    ;// D05~D00����
        u32      Min                 : 6    ;// D11~D06����
        u32      Hour                : 5    ;// D16~D12��ʱ
        u32      Day                 : 5    ;// D21~D17����
        u32      Mon                 : 4    ;// D25~D22����
        u32      Year                : 6    ;// D31~D26����
    };                                                  // 
    
    sSpeed              speed[3]            ;//�ٶ�ͨ���ź�   3*24
    
    struct {  
        u16     vcc                         ;//110V��ѹ
        u16     qy                          ;//ǣ����ѹ
        u16     xq                          ;//��ǰ��ѹ
        u16     xh                          ;//����ѹ
        u16     lw                          ;//��λ��ѹ
    }Vol;                                   //������ѹ
    
    u32  				Longitude           ;//����           4
    u32     			Latitude	        ;//γ��           4
    
    u16   				GpsSpeed       :10  ;//GPS�ٶ�         2
    u16   				GpsPos         : 3  ;//�����ϱ�����   2  1������2���ϣ�3������4���ϣ�����δ֪
    u16                 OffHightFlag   : 1  ;//�߶ȵ�����־
    u16   				Rsv            : 2  ;//����           2
    u8                  Rssi                ;//�ź�ǿ��       1
    u8                  rsvbyte[3]             ;//Ԥ���ֽ�
    
    union __u_sys_err {
        struct __s_sys_err {
        u16                  Sen1OpenErr     : 1;     //D0=1��һ�˴�����ģ�����
        u16                  Sen2OpenErr     : 1;     //D1=1�����˴�����ģ����� 
        u16                  Mtr1CommErr     : 1;     //D2=1��һ�˲���ģ��ͨ�Ź���
        u16                  Mtr2CommErr     : 1;     //D3=1�����˲���ģ��ͨ�Ź���
        u16                  OtrCommErr      : 1;     //D4=1��IC��ģ��ͨѶ����
        u16                  PwrCommErr      : 1;     //D5=1������ģ��ͨѶ����
        u16                  ExtCommErr      : 1;     //D6=1����չͨѶģ��ͨѶ����
        u16                  Disp1CommErr    : 1;     //D7=1��һ����ʾģ��ͨѶ����
        u16                  Disp2CommErr    : 1;     //D8=1��������ʾģ��ͨѶ����
        u16                  TaxCommErr      : 1;     //D9=1��   TAXͨѶ����
        u16                  ParaNullErr     : 1;     //D10=1��δ�������в���
        u16                  BoxNullErr      : 1;     //D11=1��δ��������ģ��
        u16                  DevNullErr      : 1;     //D12=1��δ����ģ��
        u16                  BoxSetErr       : 1;     //D13=1������ģ�����ô���
        u16                  FlashErr        : 1;     //D14=1���洢������
        u16                  FramErr         : 1;     //D15=1���������
        u16                  BattLowErr      : 1;     //D16=1����ص�ѹ��
        u16                  TimeErr         : 1;     //D17=1��ʱ�����
        u16                  OilLowErr       : 1;     //D18=1����λ�͹���
        u16                  OilHighErr      : 1;     //D19=1����λ�߹���
        u16                  RsvErr          : 12;     //D23~20��Ԥ��
        } Flag;
    u8                  Flags[4];
    } Err;

    u16   				CrcCheck            ;//У��   2                 
} stcFlshRec;


#endif
