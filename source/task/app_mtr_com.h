/**************************************************************
* Description  : �����ͼ�������ͨѶ
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
#ifndef  _APP_MTR_COM_H_
#define  _APP_MTR_COM_H_

#include <includes.h>

/**************************************************************
* Description  : ͨ��״̬����
* Author       : 2018/5/29 ���ڶ�, by redmorningcn
*/
#define         FREQ_CH_NONE            0x00        /* ͨ����Ƶ���ź�    */
#define         FREQ_CH1_NONE           0x01        /* ͨ��1��Ƶ���ź�   */
#define         FREQ_CH2_NONE           0x02        /* ͨ��2��Ƶ���ź�   */
#define         FREQ_CH_OK              0x03        /* ͨ��Ƶ���ź�����  */
#define         FREQ_DIFF               0x04        /* ͨ��Ƶ�����ϴ�  */

/**************************************************************
* Description  : �������ڵ��
* Author       : 2018/5/31 ������, by redmorningcn
*/
#define         NODE_SPEED1     1
#define         NODE_SPEED2     2
#define         NODE_SPEED3     3
#define         NODE_LOCO       4




/**************************************************************
* Description  : �ṹ���ٶȼ��嶨��
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
//ͨ��ʱ�������������С
#define     CH_TIMEPARA_BUF_SIZE    50
#define     CH_VOLTAGE_BUF_SIZE     20

__packed
typedef struct  
{
    uint32  id;             //��Ʒid       0
    uint32  num;            //��Ʒ���     4
    uint8   buf[64];        //Ԥ��         8
    uint32  cpu_freq;       //cpuƵ��      72
    uint32  time;           //ϵͳȫ��ʱ��(ϵͳʱ��(1/72Mhz) *65536)=Լ1ms   76
}strSpeedSys;              //size = 80
    
/**************************************************************
* Description  : �ٶȼ��
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
__packed
typedef struct
{
   /*******************************************************************************
    * Description  : ͨ������ָ��
    * Author       : 2018/3/14 ������, by redmorningcn
    *******************************************************************************/
    struct _strsignalchannelpara_ {
        uint32              period;                         //���ڣ�  0.00-2000000.00us ��0.5Hz��
        uint32              freq;                           //Ƶ�ʣ�  0-100000hz              
        uint16              raise;                          //�����أ�0.00-50.00us
        uint16              fail;                           //�½��أ�0.00-50.00us
        uint16              ratio;                          //ռ�ձȣ�0.00-100.00%
        uint16              Vol;                            //�͵�ƽ��0.00-30.00V
        uint16              Voh;                            //�ߵ�ƽ��0.00-30.00V
        uint16              status;                         //ͨ��״̬
    }para[2];
    
    uint32  ch1_2phase;                                     //��λ�0.00-360.00��
    uint16  vcc_vol;                                        //�����ѹ
    uint16  stand_vol;                                      //�ο���ѹ
}strSpeed;

/*******************************************************************************
* Description  : ͨ���ṹ�塣���У�ͨ��ʱ������������㣻
                             ͨ����������ͨ���ľ���ָ�ꡣ
* Author       : 2018/3/13 ���ڶ�, by redmorningcn
*******************************************************************************/
__packed
typedef struct
{
    /*******************************************************************************
    * Description  : ��������
    * Author       : 2018/3/14 ������, by redmorningcn
    *******************************************************************************/
    struct  {
        //ʱ��ָ��
        struct  {
            uint32  low_up_time;                            //10%λ�ã������أ��ж�ʱ�� 
            uint32  low_down_time;                          //10%λ�ã��½��أ��ж�ʱ��
            uint32  hig_up_time;                            //90%λ�ã������أ��ж�ʱ��
            uint32  hig_down_time;                          //90%λ�ã��½��أ��ж�ʱ��
            uint16  low_up_cnt;
            uint16  low_down_cnt;
            uint16  hig_up_cnt;
            uint16  hig_down_cnt;
        }time[CH_TIMEPARA_BUF_SIZE];
        
        uint16  p_write;                                    //��������д����
        uint16  p_read;
        uint32  pulse_cnt;                                  //����������ж��ź�����
        
        //�ź�����λ�ã���/��
        union  
        {
            struct __pluse_status__ {                      // �ź�״̬
                uint32  raise_10: 1;                        // ����λ�á���10%
                uint32  raise_90: 1;       	                // ����λ��--90%
                uint32  fall_90 : 1;       	                // �½�λ��--10
                uint32  fall_10 : 1;  	                    // ������δ����
                uint32  res     : 28;  	                    // ������δ����
            }station;
            uint32  pluse_status;
        };        
        
        //��ƽָ��
        struct {
            uint16  ch_low_voltage;                         //�źŵ͵�ƽ
            uint16  ch_hig_voltage;                         //�źŸߵ�ƽ
            uint16  vcc_hig_voltage;                        //ͨ�������ƽ
        }voltage[CH_VOLTAGE_BUF_SIZE];
        uint16  p_wr_vol;                                   //��ƽд��ָ��
        uint16  p_rd_vol;                                   //��ƽ����ָ��
    }test[2];                                               //ͨ���������
    
    strSpeed        speed;                             //�ٶȲ���
 
}strCoupleChannel;

/**************************************************************
* Description  : У׼����
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
typedef struct {
    u32     line;       //�������Զ�
    int16   Deviance;   //����ƫ��
    int16   tmp;        //Ԥ��

}strCalibration;


/**************************************************************
* Description  : ����������
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
typedef struct{
    union   {
        struct{
            strCalibration  VccVol;         //��ƽ
            strCalibration  Ch1Vol;         //ͨ��1��ѹ
            strCalibration  Ch2Vol;         //ͨ��2��ѹ
        };
        strCalibration      CaliBuf[20];
    };
}strCaliTable;


/**************************************************************
* Description  : �ٶȼ������ݽṹ
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
__packed
typedef union _UnnSpeedctrl_ {
    struct{
        strSpeedSys         sys;            //��������
        strCoupleChannel    ch;             //��Ʒ���в���
        strCaliTable        cali;           //У׼ϵ����
        MODBUS_CH   	    *pch;           //modbus���ƿ�
    };
    uint16   buf[512];
        
}UnnSpeedctrl;


////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
* Description  : ͨ������ָ��
* Author       : 2018/3/14 ������, by redmorningcn
*******************************************************************************/
typedef union  
{
    struct _strpara_ {          
        uint16              vcc;                           //Ԥ��
        uint16              qy;                             //ǣ��
        uint16              zd;                             //�ƶ�
        uint16              xq;                             //��ǰ
        uint16              xh;                             //���
        uint16              lw;                             //��λ
    }loco;
    uint16  parabuf[6];
}strlocoVol;

/*******************************************************************************
* Description  : �����źŽṹ��
* Author       : 2018/4/17 ���ڶ�, by redmorningcn
*******************************************************************************/
//������������С
#define VOLTAGE_BUF_SIZE     20
typedef struct
{
    /*******************************************************************************
    * Description  : ��������
    * Author       : 2018/4/17 ������, by redmorningcn
    *******************************************************************************/
    //��ƽָ��
    union  
    {
        struct {
            uint16              vcc;                    //Ԥ��
            uint16              qy;                     //ǣ��
            uint16              zd;                     //�ƶ�
            uint16              xq;                     //��ǰ
            uint16              xh;                     //���
            uint16              lw;                     //��λ
        }adc;
        uint16  buf[6];
    }vol[VOLTAGE_BUF_SIZE];       
    
    uint16  p_wr_vol;                                   //��ƽд��ָ��
    uint16  p_rd_vol;                                   //��ƽ����ָ��
        
    uint32              status;                         //״̬
    
    strlocoVol          loco;
}strLocoRunStatus;


//calibration
/**************************************************************
* Description  : У׼����
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
//typedef struct {
//    u32     line;       //�������Զ�  
//    int16   Delta;      //����ƫ��
//    int16   tmp;        //Ԥ��
//
//}strCalibration;


/**************************************************************
* Description  : ����������
* Author       : 2018/5/22 ���ڶ�, by redmorningcn
*/
typedef struct{
    union   {
        struct{
            strCalibration  VccVol;         //��ƽ
            strCalibration  QY_Vol;         //
            strCalibration  ZD_Vol;         //
            strCalibration  XQ_Vol;         //
            strCalibration  XH_Vol;         //
            strCalibration  LW_Vol;         //
        };
        strCalibration      CaliBuf[20];
    };
}strLocoCaliTable;


/*******************************************************************************
* Description  : ϵͳ���в���
* Author       : 2018/3/14 ������, by redmorningcn
*******************************************************************************/
typedef struct  
{
    uint32  id;             // ��Ʒid       0
    uint32  num;            // ��Ʒ���     4
    
    union{
        struct{
            u8  sysflg      :1; // ��ϵͳ���� 
            u8  califlg     :1; // ��У׼����
            u8  rev         :6; // Ԥ��
        };
        u8  flags;
    }paraflg;
    
    uint8   buf[63];        // Ԥ��         8
    uint32  cpu_freq;       // cpuƵ��      72
    uint32  time;           // ϵͳȫ��ʱ��(ϵͳʱ��(1/72Mhz) *65536)=Լ1ms   76
}strLocoSysPara;    

__packed
typedef union _Unnlococtrl_ {
    struct{
        strLocoSysPara      sys;            //��������
        strLocoRunStatus    loco;           //��Ʒ���в���    
        strLocoCaliTable    calitab;        //����ϵ��
    };
    uint16   buf[512];
        
}Unnlococtrl;



#endif

