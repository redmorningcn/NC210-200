/*******************************************************************************
* Description  : ���弰��ʼ��ȫ�ֱ���
* Author       : 2018/5/10 ������, by redmorningcn
*******************************************************************************/
#include <app_type.h>
#include <tasks.h>

/*******************************************************************************
* Description  : ����ȫ�ֱ���  sCtrl
* Author       : 2018/5/10 ������, by redmorningcn
*******************************************************************************/
Unnctrl     Ctrl;              //
    
/*******************************************************************************
* Description  : ���߷���ģ�����ָ��
* Author       : 2018/5/14 ����һ, by redmorningcn
*******************************************************************************/
StrCOMCtrl  * DtuCom;

/*******************************************************************************
* Description  : �ͼ��崮�ڿ���ָ��
* Author       : 2018/5/14 ����һ, by redmorningcn
*******************************************************************************/
StrCOMCtrl  * MtrCom;

/*******************************************************************************
* Description  : �ͼ��崮�ڿ���ָ��
* Author       : 2018/5/14 ����һ, by redmorningcn
*******************************************************************************/
StrCOMCtrl  * TaxCom;


/*******************************************************************************
* Description  : ȫ�ֱ�����ʼ��
* Author       : 2018/5/18 ������, by redmorningcn
*/
void app_init_sctrl(void)
{ 
//    WdtReset();
    
    /*******************************************************************************
    * Description  : ��ʼ���������Ӳ���Ĭ��ֵ
    * Author       : 2018/5/18 ������, by redmorningcn
    */    
    u8  i,j;
    for(i = 0;i <sizeof(Ctrl.ComCtrl)/sizeof(StrCOMCtrl);i++ )
    {
        Ctrl.ComCtrl[i].ConnectFlag     = 0;            //���ӱ�ʶ��0���ô��������ӣ�1�������ӡ�
        Ctrl.ComCtrl[i].ConnectTimeOut  = 5;            //��ʱʱ�䣺5���ڣ��ô��������ӣ����ݽ��գ����򴮿ڶϿ���
        Ctrl.ComCtrl[i].TimeoutEn       = 0;            //��ʱ����ֹͣ������������ʱ������
        
        for(j = 0;j< COM_CONN_NUM;j++){                 //�ô������֧�ֵ�����������ַ��Э�����𣩣�
            Ctrl.ComCtrl[i].ConnCtrl[j].Baud        = 57600;
            Ctrl.ComCtrl[i].ConnCtrl[j].Bits        = USART_WordLength_8b;
            Ctrl.ComCtrl[i].ConnCtrl[j].Parity      = USART_Parity_No;
            Ctrl.ComCtrl[i].ConnCtrl[j].Stops       = USART_StopBits_1;
            
            Ctrl.ComCtrl[i].ConnCtrl[j].TimeOut     = 5;            //���ӳ�ʱʱ�䡣��������ʱ�䣬��Ϊ���ӶϿ���
            Ctrl.ComCtrl[i].ConnCtrl[j].EnableFlg   = 1;            //����������
            Ctrl.ComCtrl[i].ConnCtrl[j].ErrFlg      = 0;            //��������
            Ctrl.ComCtrl[i].ConnCtrl[j].MasterAddr  = LKJ_MAINBOARD_ADDR;     //������ַ
            Ctrl.ComCtrl[i].ConnCtrl[j].SlaveAddr   = DTU_ADDR;     //���ݽ��շ�������ַ
            Ctrl.ComCtrl[i].ConnCtrl[j].SendFlg     = 0;
            Ctrl.ComCtrl[i].ConnCtrl[j].RecvEndFlg  = 0;
            Ctrl.ComCtrl[i].ConnCtrl[j].SendFramNum = 1;
        }
    }

    /*****************************************************************
    * Description  : ��FRAM�зֱ����head��NumMgr��Porduct��RunPara
    * Author       : 2018/5/17 ������, by redmorningcn
    */
    Ctrl.sRunPara.FramFlg.RdHead    = 1;
    Ctrl.sRunPara.FramFlg.RdNumMgr  = 1;
    Ctrl.sRunPara.FramFlg.RdProduct = 1;
    Ctrl.sRunPara.FramFlg.RdRunPara = 1;
    App_FramPara();
    
    Ctrl.sRunPara.Err.Errors        = 0;                //������ϴ���
    
    if(Ctrl.sHeadInfo.Password != MODBUS_PASSWORD){     //ͨѶ����
        Ctrl.sHeadInfo.Password = MODBUS_PASSWORD;
        Ctrl.sRunPara.Err.FramErr = 1;                  //������ϣ�ָ����ַ������ֵ����
    }
   
    Ctrl.sRunPara.SysSts.StartFlg   = 1;                //��ʼ��ʾ 
    
    if ( ( Ctrl.sRunPara.StoreTime < 5 ) ||             //���ݴ洢����
         ( Ctrl.sRunPara.StoreTime > 10*60 ) ) {
        Ctrl.sRunPara.StoreTime    = 60;
    }
}
