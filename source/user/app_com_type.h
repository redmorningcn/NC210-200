/*******************************************************************************
* Description  : ����ͨѶ���ƽṹ�嶨�壨APP�㣩
* Author       : 2018/5/11 ������, by redmorningcn
*******************************************************************************/

#ifndef	__APP_COM_TYPE_H__
#define	__APP_COM_TYPE_H__
#include <includes.h>
#include <csnr_package_deal.h>

/*********************************************************************
* INCLUDES
*/
#define COMM_RECV_DATA_MAX_LEN   128
#define COMM_SEND_DATA_MAX_LEN   128
#define COM_CONN_NUM             4     


//���ڵ�ַ����    
#define     DTU                 0               /* DTU���ÿ��ƽṹ�����         ComCtrl[]*/
#define     DTU_C0              0               /* DTU_C0��ַ��ռ�õ����ӱ��    ConnCtrl[]*/
#define     DTU_NODE            1               /* DTU modebus���*/

//���ڵ�ַ����    
#define     MTR                 1               /* MTR���ÿ��ƽṹ�����         ComCtrl[]*/
#define     MTR_C0              0               /* MTR_C0��ַ��ռ�õ����ӱ��    ConnCtrl[]*/
#define     MTR_NODE            1               /* MTR modebus���*/


//���ڵ�ַ����    
#define     TAX                 2               /* TAX���ÿ��ƽṹ�����         ComCtrl[]*/
#define     TAX_C0              0               /* TAX_C0��ַ��ռ�õ����ӱ��    ConnCtrl[]*/
#define     TAX_NODE            1               /* TAX modebus���*/

//csnc��ַ����
#define     LKJ_MAINBOARD_ADDR  (0x84)          /* LKJ�ӿ����ߴ������ CSNC Э���ַ*/
#define     DTU_ADDR            (0xCA)          /* ���߷���ģ�� CSNC Э���ַ */


//ͨѶ��������
#define     DATA_COMM           0
#define     SET_COMM            1
#define     IAP_COMM            2

//ͨѶЭ������
#define     MODBUS_PROTOCOL     0
#define     CSNC_PROTOCOL       1

//����ͨѶ��ʱʱ��
#define     DTU_TIMEOUT         60
#define     MTR_TIMEOUT         5
#define     TAX_TIMEOUT         5


/*********************************************************************
* CONSTANTS
*/


/*********************************************************************
* TYPEDEFS
*/

/*******************************************************************************
* Description  : ���ڽ������ݽṹ������
��ͬЭ�飬��ָͬ������ݿɸ��á�
* Author       : 2018/5/11 ������, by redmorningcn
*******************************************************************************/
__packed
typedef union {
    
//  ���ݽṹ1
//  ���ݽṹ2
    u16             Buf16[COMM_RECV_DATA_MAX_LEN/2];		            //	
    u8              Buf[COMM_RECV_DATA_MAX_LEN];		            //	
} uRecvData;

/*******************************************************************************
* Description  : ���ڷ�������������
* Author       : 2018/5/11 ������, by redmorningcn
*******************************************************************************/
__packed
typedef union {
//	stcFlshRecNdp02A        sRecA;				    //���ݼ�¼     128 	
//  ���ݽṹ1
//  ���ݽṹ2
    u16             Buf16[COMM_SEND_DATA_MAX_LEN/2];		            //	
    u8              Buf[COMM_SEND_DATA_MAX_LEN];	
} uSendData;

/*******************************************************************************
* Description  : ���ӿ�����
* Author       : 2018/5/11 ������, by redmorningcn
*******************************************************************************/
__packed
typedef struct {     
    u8      EnableFlg   :1;     //���ӿ��ƣ�1���������ӣ�0������������
    u8      RecvEndFlg  :1;		//���ձ�ʾ��1�����ݽ�����ɣ�0�������ݽ��ա�
    u8      ConnType    :2;     //�������ͣ�0�����ݴ��䣻1���������ã�2��IAP���䣻
    u8      SendFlg     :1;     //���ͱ�ʾ�������ݷ��ͣ�1�������ݷ��ͣ�0
    u8      ErrFlg      :1;     //�����ʾ������������0�����Ӵ���1
    u8      protocol    :2;     //ͨ��Э�顣0��modbus��1��csnc��

    u8      TimeOutEn   :1;     //��ʱ�����������ʶ��
    u8      Connflg     :1;     //����״̬��1�������ӣ�0�������ӡ�
    u8      TimeOut     :7;     //��ʱʱ�䣬��λ1s��
    union {
        struct{
            u8      DestAddr;       //Դ��ַ        master = 0x80	   
            u8      SourceAddr;     //���յ�ַ      slave  = 0xCA	   
            u8      FramNum;        //֡���   
            u8      FrameCode;      //֡������
        };
        
        strCsnrProtocolPara sCsnc;  //CSNCЭ��ṹ��
    };
    u8      MB_Node     :5;         //modbus���ӱ��
    u8      COM_Num     :3;         //���ڱ��

    u8      Bits        :4;
    u8      Parity      :2;
    u8      Stops       :2;
    
    u32     Baud;
    u32     DataCode;               //�����֣��������ڲ���
} sCOMConnCtrl;		


//���տ�����
typedef struct {     
    union {
        struct{
            u8      DestAddr;     //Դ��ַ        master = 0x80	   
            u8      SourceAddr;      //���յ�ַ      slave  = 0xCA	   
            u8      FramNum;        //֡���   
            u8      FrameCode;      //֡������
        };
        
        strCsnrProtocolPara sCsnc;  //CSNCЭ��ṹ��
    };
    u8      Len;                    //������Ч���ݳ���
    u8      protocol;               //ͨ��Э�顣0��modbus��1��csnc��
    u8      EvtFlag;                //ʱ���ʶ��
    u8      Tmp[1];
    u32     DataCode;               //���տ�����
    
} sCOMRecvCtrl;

/*******************************************************************************
* Description  : ���ڿ��ƽṹ�壨app�㣩
* Author       : 2018/5/11 ������, by redmorningcn
*******************************************************************************/
__packed
typedef struct {  
	uRecvData	        Rd;			                    //����������
	uSendData	        Wr;			                    //����������
    
    MODBUS_CH           *pch;                   		// MODBUS���
    
    u16                 ConnectTimeOut  : 14;     	    // ���ӳ�ʱ����������Ϊ��λ
    u16                 TimeoutEn       : 1;         	// ����������
    u16                 ConnectFlag     : 1;         	// ���ӱ�־

    /***************************************************
    * ������ ���ڿ�����
    */
	sCOMRecvCtrl    	RxCtrl;				            //���տ��ƣ�������ǰ���յ��Ŀ�����Ϣ 
	sCOMConnCtrl		ConnCtrl;                       //���ӿ��ƣ���ÿ����ַ��Ϊ�������������ӣ���APP���ö˴���
	
} StrCOMCtrl;


#endif	
