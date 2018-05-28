/**************************************************************
* Description  : ��������Э��
* Author       : 2018/5/25 ������, by redmorningcn
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
* Description  : app_iap_deal  ���Զ�̻򴮿�����
* Author       : 2018/5/25 ������, by redmorningcn
*/
void    app_iap_deal(void){
    stcIAPPara      sLocalIap;                          //�洢��iap����
    stcIAPPara      *sServerIap = (stcIAPPara *)&DtuCom->Rd.dtu.iap;  //�������·���iap���� 

    u8      code = DtuCom->Rd.dtu.iap.code;             //IAPָ��λ
    u8      sta  = DtuCom->Rd.dtu.iap.sta;              //IAP״̬λ
    u16     crc16;
    u16     idx;                                        //���ݱ��
    u16     datalen;
    
    switch(code)
    {
        
    case IAP_START:                                     //IAP����,
        //���Ѵ��iappara��Ϣ
        //memcpy((uint8 *)&sLocalIap,(uint8 *)IAP_PARA_START_ADDR,sizeof(stcIAPPara));
        BSP_FlashReadBytes(IAP_PARA_START_ADDR,(uint8 *)&sLocalIap,sizeof(stcIAPPara));
        crc16 = GetCrc16Chk((uint8 *)&sLocalIap,sizeof(sLocalIap)-8);
        
        //�ԱȽ��յ���iappara��Ϣ���汾����С���ѷ�֡��š�У�顢֡�ţ�
        if ( ( sta == 1 )                           ||      // ��������
            ( sLocalIap.Idx    == 0xffffffff ) ||          // �洢���Ϊ-1
            ( sLocalIap.Idx    == 0 )          ||          // �洢���Ϊ��
            ( sServerIap->Idx  == 0 )          ||          // �������������Ϊ��
            ( sLocalIap.Chk    != crc16)       ||          // У���벻��ȷ
            ( sServerIap->Size != sLocalIap.Size ) ||      // �����С����
                                //( IapServer.Crc32 != IapInfo.Crc32 ) 
            ( sServerIap->SwVer!= sLocalIap.SwVer )        // �汾����
            ) 
        {
            /**************************************************************
            * Description  : ���ط���������
            * Author       : 2018/5/28 ����һ, by redmorningcn
            */
            sLocalIap.Addr        = USER_BACK_START_ADDR;       //��ʼ����ַ
            sLocalIap.Idx         = 0;
            
            sLocalIap.HwVer       = Ctrl.sProductInfo.HwVer;    //����װ�ð汾��Ϣ
            sLocalIap.SwVer       = Ctrl.sProductInfo.SwVer;
            sLocalIap.Size        = sServerIap->Size;           
            sLocalIap.Crc32       = sServerIap->Crc32;
        }else{
            /**************************************************************
            * Description  : ���������洢�ĵ�ַ����
            * Author       : 2018/5/28 ����һ, by redmorningcn
            */
            //sLocalIap.HwVer       = Ctrl.sProductInfo.HwVer;  //���������İ汾��Ϣ
            //sLocalIap.SwVer       = Ctrl.sProductInfo.SwVer;  
            sLocalIap.Addr        = USER_BACK_START_ADDR + sLocalIap.Idx * 128;
        }
        
        //��������Ϣ���ظ�����������Ӳ���汾������汾�������С����ǰ��ַ����ǰ֡��
        memcpy((u8 *)&DtuCom->Rd.dtu.iap,(u8 *)&sLocalIap,sizeof(stcIAPPara)); 
        //δ���͸�д���򣬲��洢���ĺ��sLocalIapֵ��
        
        break;
    case IAP_DATA:                                      //IAP���ݴ���Ӧ��
        sLocalIap.End   =   0;                          //End=1������������ɣ�End=0��δ��
        if(DtuCom->ConnCtrl.datalen < 4){
            DtuCom->Rd.dtu.iap.sta  = IAP_IDX_ERR;
            break;
        }
        datalen = DtuCom->ConnCtrl.datalen - 4;         //��֡���򳤶ȡ���code��1����sta��1����idx��2��
        idx = DtuCom->Rd.dtu.iap.idx;                   //���
        
        DtuCom->Rd.dtu.iap.sta = IAP_IDX_ERR;           //Ӧ��Ĭ������
        
        /**************************************************************
        * Description  : �����䣬����idxЭ������ͬ����
        ���յ�idx������յ�idx��ͬ��������ȷ��
        * Author       : 2018/5/28 ����һ, by redmorningcn
        */
        if(idx == sLocalIap.Idx)                        //�����ȣ��������͡�
        {
            BSP_FlashWriteBytes(sLocalIap.Addr,DtuCom->Rd.dtu.iap.buf,datalen); //������д��ָ����ַ
            
            sLocalIap.Addr = sLocalIap.Idx + datalen;
            sLocalIap.Idx++;                                    //���++���������½��С�
            
            u32 size = sLocalIap.Addr - USER_BACK_START_ADDR;   //�����س����С
            if(     size > sLocalIap.Size                       //�ѷ��͵ĳ����С�Ѿ����ڳ����С��
               ||   sLocalIap.Size > USER_BACK_PRO_SIZE)        //������ڴ洢����
            {
                DtuCom->Rd.dtu.iap.sta = IAP_SIZE_ERR;          //�����С���� 
            }
            //�����
            crc16 = GetCrc16Chk((uint8 *)&sLocalIap,sizeof(sLocalIap)-8);
            sLocalIap.Chk = crc16;
            BSP_FlashWriteBytes(IAP_PARA_START_ADDR,(uint8 *)&sLocalIap,sizeof(stcIAPPara));

        }else if(idx == sLocalIap.Idx -1){                      //�ظ�֡����������һ֡�ظ����ͣ�

        }else{                                                  //֡��Ŵ���
            DtuCom->Rd.dtu.iap.sta  = IAP_IDX_ERR;
        }
        
        DtuCom->Rd.dtu.iap.replyIdx = sLocalIap.Idx;            //Ӧ��ǰ��š�    
        
        break;
    case IAP_END:
        //����ʾ������ʾ��ͣ2����
        App_DispDelay(2000); 
        
        //������������жϡ����Ѵ��С�ͳ����С�����
        if(     sLocalIap.SwVer == DtuCom->Rd.dtu.iap.para.SwVer//����汾���
           ||   sLocalIap.Addr  == DtuCom->Rd.dtu.iap.para.Size //��д���ݴ�С���ڳ����С
           )    
        {

            sLocalIap.End = 1;              //�Ѿ��������

            /**************************************************************
            * Description  : ���ݳ����С��ȷ���Ƿ�����Ϊboot��
            �������С��USER_BOOT_PRO_SIZE����Ϊ��boot����д��boot����
            * Author       : 2018/5/28 ����һ, by redmorningcn
            */
            if(sLocalIap.Size +  2048 < USER_BOOT_PRO_SIZE )
            {
                u32 framnum     = (sLocalIap.Size + IAP_DATA_BUF_LEN -1)/ IAP_DATA_BUF_LEN;
                u32 bootaddr    = USER_BOOT_START_ADDR;     //boot��ʼ��ַ
                u32 backupaddr  = USER_BACK_START_ADDR;
                for(u32 i= 0;i< framnum ;i++){
                    BSP_FlashReadBytes(backupaddr,DtuCom->Rd.dtu.iap.buf,IAP_DATA_BUF_LEN);
                    BSP_FlashWriteBytes(bootaddr,DtuCom->Rd.dtu.iap.buf,IAP_DATA_BUF_LEN);
                    bootaddr    += IAP_DATA_BUF_LEN;
                    backupaddr  += IAP_DATA_BUF_LEN;
                }
                sLocalIap.End = 0;                          //дboot�������ʶ   
                uprintf("boot ok");
            }else{
                uprintf("app ok");
            }
            
            /**************************************************************
            * Description  : �������
            * Author       : 2018/5/28 ����һ, by redmorningcn
            */
            sLocalIap.Idx = 0;          //�������
            //�����
            crc16 = GetCrc16Chk((uint8 *)&sLocalIap,sizeof(sLocalIap)-8);
            sLocalIap.Chk = crc16;
            BSP_FlashWriteBytes(IAP_PARA_START_ADDR,(uint8 *)&sLocalIap,sizeof(stcIAPPara));
        }else{
            DtuCom->Rd.dtu.iap.sta  = IAP_END_ERR;      //����ʱ����Ϣ����
            uprintf("iap err.5");
        }
        
        break;
    default:
        DtuCom->Rd.dtu.iap.sta  = IAP_CODE_ERR;         //ָ�����
        break;
    }
}


/*******************************************************************************
 * 				end of file                                                    *
 *******************************************************************************/