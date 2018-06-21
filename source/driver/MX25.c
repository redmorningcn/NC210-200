/*******************************************************************************
* Description  : Flash��driver����
* Author       : 2018/5/15 ���ڶ�, by redmorningcn
*******************************************************************************/
#include  <mx25.h>


#define SPI_FLASH_SPI                   SPI1
#define SPI_FLASH_SPI_CLK               RCC_APB2Periph_SPI1

#define SPI_FLASH_SPI_SCK_PIN           GPIO_Pin_5              /* PB.13 */
#define SPI_FLASH_SPI_SCK_GPIO_PORT     GPIOA
#define SPI_FLASH_SPI_SCK_GPIO_CLK      RCC_APB2Periph_GPIOA

#define SPI_FLASH_SPI_MISO_PIN          GPIO_Pin_6              /* PB.14 */
#define SPI_FLASH_SPI_MISO_GPIO_PORT    GPIOA
#define SPI_FLASH_SPI_MISO_GPIO_CLK     RCC_APB2Periph_GPIOA

#define SPI_FLASH_SPI_MOSI_PIN          GPIO_Pin_7              /* PB.15 */
#define SPI_FLASH_SPI_MOSI_GPIO_PORT    GPIOA
#define SPI_FLASH_SPI_MOSI_GPIO_CLK     RCC_APB2Periph_GPIOA

#define SPI_FLASH_CS_PIN_NUM            4                       /* PB.12 */
#define SPI_FLASH_CS_PIN                GPIO_Pin_4  
#define SPI_FLASH_CS_GPIO_PORT          GPIOA
#define SPI_FLASH_CS_GPIO_CLK           RCC_APB2Periph_GPIOA

/* Private typedef -----------------------------------------------------------*/
#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256


/* Private define ------------------------------------------------------------*/
#define W25X_WriteEnable                0x06 
#define W25X_WriteDisable               0x04 
#define W25X_ReadStatusReg              0x05 
#define W25X_WriteStatusReg             0x01 
#define W25X_ReadData                   0x03 
#define W25X_FastReadData               0x0B 
#define W25X_FastReadDual               0x3B 
#define W25X_PageProgram                0x02 
#define W25X_BlockErase                 0xD8 
#define W25X_SectorErase                0x20 
#define W25X_ChipErase                  0xC7 
#define W25X_PowerDown                  0xB9 
#define W25X_ReleasePowerDown           0xAB 
#define W25X_DeviceID                   0xAB 
#define W25X_ManufactDeviceID           0x90 
#define W25X_JedecDeviceID              0x9F 

#define WIP_FlagMask                    0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte1                     0xA5


/* Private macro -------------------------------------------------------------*/
/* Select SPI FLASH: Chip Select pin low  */
#define CE_Low()       GPIO_ResetBits(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_PIN)//(Periph_BB((u32)&SPI_FLASH_CS_GPIO_PORT->ODR, SPI_FLASH_CS_PIN_NUM) = 0)

/* Deselect SPI FLASH: Chip Select pin high */
#define CE_High()      GPIO_SetBits(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_PIN)//(Periph_BB((u32)&SPI_FLASH_CS_GPIO_PORT->ODR, SPI_FLASH_CS_PIN_NUM) = 1)

#define  UCOS_EN        DEF_ENABLED

/***********************************************
* ������ OS�ӿ�
*/
#if UCOS_EN     == DEF_ENABLED
    #if OS_VERSION > 30000U
    static  OS_SEM                   Bsp_EepSem;    // �ź���
    #else
    static  OS_EVENT                *Bsp_EepSem;    // �ź���
    #endif
#endif
  
    
/*******************************************************************************
* ��    �ƣ� EEP_WaitEvent
* ��    �ܣ� �ȴ��ź���
* ��ڲ����� ��
* ���ڲ�����  0���������󣩣�1�������ɹ���
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ����ʹ��UCOS����ϵͳʱʹ��
*******************************************************************************/
static u8 EEP_WaitEvent(void)
{
    /***********************************************
    * ������ OS�ӿ�
    */
#if OS_VERSION > 30000U
    return BSP_OS_SemWait(&Bsp_EepSem,0);           // �ȴ��ź���
#else
    u8       err;
    OSSemPend(Bsp_EepSem,0,&err);                   // �ȴ��ź���
    if ( err = OS_ERR_NONE )
      return TRUE;
    else
      return FALSE;
#endif
}


/*******************************************************************************
* ��    �ƣ� EEP_SendEvent
* ��    �ܣ� �ͷ��ź���
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ����ʹ��UCOS����ϵͳʱʹ��
*******************************************************************************/
static void EEP_SendEvent(void)
{
    /***********************************************
    * ������ OS�ӿ�
    */
#if OS_VERSION > 30000U
    BSP_OS_SemPost(&Bsp_EepSem);                        // �����ź���
#else
    u8       err;
    OSSemPost(Bsp_EepSem);                              // �����ź���
#endif
}

/**-----------------------------------------------------------------
  * @������ SPI_FLASH_Init
  * @����   ��ʼ�����ⲿSPI FLASH�ӿڵ���������
  *         Initializes the peripherals used by the SPI FLASH driver.
  * @����   ��
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_FLASH_Init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable SPI2 and GPIO clocks */
    /*!< SPI_FLASH_SPI_CS_GPIO, SPI_FLASH_SPI_MOSI_GPIO, 
       SPI_FLASH_SPI_MISO_GPIO, SPI_FLASH_SPI_DETECT_GPIO 
       and SPI_FLASH_SPI_SCK_GPIO Periph clock enable */
    RCC_APB2PeriphClockCmd(SPI_FLASH_CS_GPIO_CLK | SPI_FLASH_SPI_MOSI_GPIO_CLK |
                         SPI_FLASH_SPI_MISO_GPIO_CLK | SPI_FLASH_SPI_SCK_GPIO_CLK, ENABLE);

    /*!< SPI_FLASH_SPI Periph clock enable */
    RCC_APB2PeriphClockCmd(SPI_FLASH_SPI_CLK, ENABLE);
    
    /*!< AFIO Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    /*!< Configure SPI_FLASH_SPI pins: SCK */
    GPIO_InitStructure.GPIO_Pin = SPI_FLASH_SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			   //��������Ϊ�������������
    GPIO_Init(SPI_FLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure SPI_FLASH_SPI pins: MOSI */
    GPIO_InitStructure.GPIO_Pin = SPI_FLASH_SPI_MOSI_PIN;
    GPIO_Init(SPI_FLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure SPI_FLASH_SPI pins: MISO */
    GPIO_InitStructure.GPIO_Pin = SPI_FLASH_SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                       //GPIO_Mode_IN_FLOATING;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;				    //����Ϊ����PP��OD����
    GPIO_Init(SPI_FLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure SPI_FLASH_SPI_CS_PIN pin: SPI_FLASH Card CS pin */
    GPIO_InitStructure.GPIO_Pin = SPI_FLASH_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(SPI_FLASH_CS_GPIO_PORT, &GPIO_InitStructure);

    /* Deselect the FLASH: Chip Select high */
    CE_High();

    /* SPI2 configuration */
    // W25X16: data input on the DIO pin is sampled on the rising edge of the CLK. 
    // Data on the DO and DIO pins are clocked out on the falling edge of CLK.
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    /* Enable SPI2  */
    SPI_Cmd(SPI1, ENABLE);

    SPI_Flash_WAKEUP();
    
    /***********************************************
    * ������ OS�ӿ�
    */
#if (UCOS_EN     == DEF_ENABLED)
#if OS_VERSION > 30000U
    BSP_OS_SemCreate(&Bsp_EepSem,1, "Bsp EepSem");      // �����ź���
#else
    Bsp_EepSem     = OSSemCreate(1);                    // �����ź���
#endif
#endif
}

/**-----------------------------------------------------------------
  * @������ Send_Byte
  * @����   ͨ��SPI���߷���һ���ֽ�����(˳�����һ���ֽ�����)
  *         Sends a byte through the SPI interface and return the byte
  *         received from the SPI bus.
  * @����   Ҫд���һ���ֽ�����
  * @����ֵ �ڷ�����ʱ��MISO�ź����Ͻ��յ�һ���ֽ�
***----------------------------------------------------------------*/
u8 Send_Byte(u8 byte)
{
    /* Loop while DR register in not empty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

    /* Send byte through the SPI2 peripheral */
    SPI_I2S_SendData(SPI1, byte);

    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI1);
}

/**-----------------------------------------------------------------
  * @������ Get_Byte
  * @����   ��ȡSPI FLASH��һ���ֽڣ�δ�������Ͷ��������ʼ��ַ
  * @����   ��
  * @����ֵ ��SPI_FLASH��ȡ��һ���ֽ�
***----------------------------------------------------------------*/
u8 Get_Byte(void)
{
    return (Send_Byte(Dummy_Byte1));
}

/**-----------------------------------------------------------------
  * @������ SPI_Flash_WAKEUP
  * @����   ����SPI FLASH
  * @����   ��
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_Flash_WAKEUP(void)   
{
  /* Select the FLASH: Chip Select low */
  CE_Low();

  /* Send "Power Down" instruction */
  Send_Byte(W25X_ReleasePowerDown);

  /* Deselect the FLASH: Chip Select high */
  CE_High();
}   
 
/* ���º�������ֲʱ�����޸� */
/************************************************************************
** ��������:MX25L1602_RD                                                                                                        
** ��������:MX25L1602�Ķ�����,��ѡ���ID�Ͷ����ݲ���                
** ��ڲ���:
**            u32 Dst��Ŀ���ַ,��Χ 0x0 - MAX_ADDR��MAX_ADDR = 0x1FFFFF��                
**            u32 NByte:    Ҫ��ȡ�������ֽ���
**            u8* RcvBufPt:���ջ����ָ��            
** ���ڲ���:�����ɹ��򷵻�OK,ʧ���򷵻�ERROR        
** ע     ��:��ĳ������,ĳһ��ڲ�����Ч,���ڸ���ڲ�������Invalid���ò�����������
************************************************************************/
u8 MX25L3206_RD(u32 Dst, u32 NByte,u8* RcvBufPt)
{
    u32 i = 0;
	u32 adr;
    
    EEP_WaitEvent();
    
	//adr=Dst&MAX_ADDR ;
    adr=Dst ;
    CE_Low();            
    Send_Byte(0x0B);                                // ���Ͷ�����
    Send_Byte(((adr & 0xFFFFFF) >> 16));            // ���͵�ַ��Ϣ:�õ�ַ��3���ֽ����
    Send_Byte(((adr & 0xFFFF) >> 8));
    Send_Byte(adr & 0xFF);
    Send_Byte(0xFF);                                // ����һ�����ֽ��Զ�ȡ����
    for (i = 0; i < NByte; i++)        
    {
        RcvBufPt[i] = Get_Byte();        
    }
    CE_High();   
    EEP_SendEvent();
    return (1);
}

/************************************************************************
** ��������:MX25L1602_RdID                                                                                                        
** ��������:MX25L1602�Ķ�ID����,��ѡ���ID�Ͷ����ݲ���                
** ��ڲ���:
**            idtype IDType:ID���͡��û�����Jedec_ID,Dev_ID,Manu_ID������ѡ��
**            u32* RcvbufPt:�洢ID������ָ��
** ���ڲ���:�����ɹ��򷵻�OK,ʧ���򷵻�ERROR        
** ע     ��:������Ĳ���������Ҫ���򷵻�ERROR
************************************************************************/
u32 MX25L3206_RdID(void)
{
    u32 temp = 0;
    CE_Low();            
    Send_Byte(0x9F);                            // ���Ͷ�JEDEC ID����(9Fh)
    temp = (temp | Get_Byte()) << 8;            // �������� 
    temp = (temp | Get_Byte()) << 8;    
    temp = (temp | Get_Byte());                 // �ڱ�����,temp��ֵӦΪ0xBF2541
    CE_High();            
	return temp;
}

/************************************************************************
** ��������:MX25L1602_WR                                            
** ��������:MX25L1602��д��������д1���Ͷ�����ݵ�ָ����ַ                                    
** ��ڲ���:
**            u32 Dst��Ŀ���ַ,��Χ 0x0 - MAX_ADDR��MAX_ADDR = 0x1FFFFF��                
**            u8* SndbufPt:���ͻ�����ָ��
**          u32 NByte:Ҫд�������ֽ���
** ���ڲ���:�����ɹ��򷵻�OK,ʧ���򷵻�ERROR        
** ע     ��:��ĳ������,ĳһ��ڲ�����Ч,���ڸ���ڲ�������Invalid���ò�����������
************************************************************************/
u8 MX25L3206_WR(u32 Dst, u8* SndbufPt, u32 NByte)
{
    u8 StatRgVal = 0;
    u32 i = 0;
	u32 adr;
    
    EEP_WaitEvent();
    
	//adr=Dst&MAX_ADDR ;
    adr=Dst ;
    
    CE_Low();            
    Send_Byte(0x06);                                                    /* ����дʹ������               */
    CE_High();        
    //д���� 
    CE_Low();            
    Send_Byte(0x02);                                                    /* �����ֽ�������д����         */
    Send_Byte((((adr+i) & 0xFFFFFF) >> 16));                            /* ����3���ֽڵĵ�ַ��Ϣ        */
    Send_Byte((((adr+i) & 0xFFFF) >> 8));
    Send_Byte((adr+i) & 0xFF);
    for(i = 0; i < NByte; i++)
    {
        Send_Byte(SndbufPt[i]);                                     /* ���ͱ���д������             */
    }
    CE_High();    
    
    do
    {
        CE_Low();             
        Send_Byte(0x05);                                                /* ���Ͷ�״̬�Ĵ�������         */
        StatRgVal = Get_Byte();                                         /* ������õ�״̬�Ĵ���ֵ       */
        CE_High();                                
    }
    while (StatRgVal == 0x03);                                          /* һֱ�ȴ���ֱ��оƬ����       */
    EEP_SendEvent();
    return (1);        
}

/************************************************************************
** ��������:MX25L1602_Erase                                                
** ��������:����ָ����������ѡȡ���Ч���㷨����                                
** ��ڲ���:
**            u32 sec1����ʼ������,��Χ(0~499)
**            u32 sec2����ֹ������,��Χ(0~499)
** ���ڲ���:�����ɹ��򷵻�OK,ʧ���򷵻�ERROR        
************************************************************************/

u8 MX25L3206_Erase(u32 sec1, u32 sec2)
{
    u8  temp2 = 0,StatRgVal = 0;
    u32 SecnHdAddr = 0;                  
    u32 no_SecsToEr = 0;                       // Ҫ������������Ŀ
    u32 CurSecToEr = 0;                        // ��ǰҪ������������
    
    /*  �����ڲ��� */
    if ((sec1 > SEC_MAX)||(sec2 > SEC_MAX))    
    {
        return (0);    
    }       
    
    CE_Low();            
    Send_Byte(0x06);                                // ����дʹ������
    CE_High();            

    /* ����û��������ʼ�����Ŵ�����ֹ�����ţ������ڲ��������� */
    if (sec1 > sec2)
    {
       temp2 = sec1;
       sec1  = sec2;
       sec2  = temp2;
    } 
    /* ����ֹ���������������������� */
    if (sec1 == sec2)    
    {
        SecnHdAddr = SEC_SIZE * sec1;               // ������������ʼ��ַ
        CE_Low();    
        Send_Byte(0x20);                            // ������������ָ��
        Send_Byte(((SecnHdAddr & 0xFFFFFF) >> 16)); // ����3���ֽڵĵ�ַ��Ϣ
           Send_Byte(((SecnHdAddr & 0xFFFF) >> 8));
           Send_Byte(SecnHdAddr & 0xFF);
          CE_High();            
        do
        {
            CE_Low();             
            Send_Byte(0x05);                        // ���Ͷ�״̬�Ĵ�������
            StatRgVal = Get_Byte();                 // ������õ�״̬�Ĵ���ֵ
            CE_High();                                
          }
        while (StatRgVal == 0x03);                  // һֱ�ȴ���ֱ��оƬ����
        return (1);            
    }
    
                     /*         ������ʼ��������ֹ��������������ٵĲ�������                             */    
    
    if (sec2 - sec1 == SEC_MAX)    
    {
        CE_Low();            
        Send_Byte(0x60);                            // ����оƬ����ָ��(60h or C7h)
        CE_High();            
        do
        {
            CE_Low();             
            Send_Byte(0x05);                        // ���Ͷ�״̬�Ĵ�������
            StatRgVal = Get_Byte();                 // ������õ�״̬�Ĵ���ֵ
            CE_High(); 
			//Delay(10);								//�˴���ʱ�Ƿ��Ǹ���
        }
        while (StatRgVal == 0x03);                  // һֱ�ȴ���ֱ��оƬ����
        return (1);
    }
    
    no_SecsToEr = sec2 - sec1 +1;                   // ��ȡҪ������������Ŀ
    CurSecToEr  = sec1;                             // ����ʼ������ʼ����
    
    /* ����������֮��ļ���������ȡ16���������㷨 */
    while (no_SecsToEr >= 16)
    {
        SecnHdAddr = SEC_SIZE * CurSecToEr;         // ������������ʼ��ַ
        CE_Low();    
        Send_Byte(0xD8);                            // ����64KB�����ָ��
        Send_Byte(((SecnHdAddr & 0xFFFFFF) >> 16)); // ����3���ֽڵĵ�ַ��Ϣ
        Send_Byte(((SecnHdAddr & 0xFFFF) >> 8));
        Send_Byte(SecnHdAddr & 0xFF);
        CE_High();            
        do
        {
            CE_Low();             
            Send_Byte(0x05);                        // ���Ͷ�״̬�Ĵ�������
            StatRgVal = Get_Byte();                 // ������õ�״̬�Ĵ���ֵ
            CE_High();                                
          }
        while (StatRgVal == 0x03);                  // һֱ�ȴ���ֱ��оƬ����
        CurSecToEr  += 16;                          // ���������16��������,�Ͳ����������ڵĴ�����������
        no_SecsToEr -=  16;                         // �������������������������
    }
    /* ����������֮��ļ���������ȡ8���������㷨 */
    while (no_SecsToEr >= 8)
    {
        SecnHdAddr = SEC_SIZE * CurSecToEr;         // ������������ʼ��ַ
        CE_Low();    
        Send_Byte(0x52);                            // ����32KB����ָ��
        Send_Byte(((SecnHdAddr & 0xFFFFFF) >> 16)); // ����3���ֽڵĵ�ַ��Ϣ
        Send_Byte(((SecnHdAddr & 0xFFFF) >> 8));
        Send_Byte(SecnHdAddr & 0xFF);
        CE_High();            
        do
        {
            CE_Low();             
            Send_Byte(0x05);                        // ���Ͷ�״̬�Ĵ�������
            StatRgVal = Get_Byte();                 // ������õ�״̬�Ĵ���ֵ
            CE_High();                                
          }
        while (StatRgVal == 0x03);                  // һֱ�ȴ���ֱ��оƬ����    
        CurSecToEr  += 8;
        no_SecsToEr -=  8;
    }
    /* �������������㷨����ʣ������� */
    while (no_SecsToEr >= 1)
    {
        SecnHdAddr = SEC_SIZE * CurSecToEr;         // ������������ʼ��ַ
        CE_Low();    
        Send_Byte(0x20);                            // ������������ָ��
        Send_Byte(((SecnHdAddr & 0xFFFFFF) >> 16)); // ����3���ֽڵĵ�ַ��Ϣ
           Send_Byte(((SecnHdAddr & 0xFFFF) >> 8));
           Send_Byte(SecnHdAddr & 0xFF);
          CE_High();            
        do
        {
              CE_Low();             
            Send_Byte(0x05);                        // ���Ͷ�״̬�Ĵ�������
            StatRgVal = Get_Byte();                 // ������õ�״̬�Ĵ���ֵ
            CE_High();                                
          }
        while (StatRgVal == 0x03);                  // һֱ�ȴ���ֱ��оƬ����
        CurSecToEr  += 1;
        no_SecsToEr -=  1;
    }
    return (1);
}

/*******************************************************************************
* Description  : FLASHд������д���ȫ1����ǰ������в���������
* Author       : 2018/5/15 ���ڶ�, by redmorningcn
*******************************************************************************/
u8 WriteFlsh(u32 adr,u8 *buf,u32 len)
{
    u32     i = 0;
    /*******************************************************************************
    * Description  : ����д����ǰ���ж������Ƿ����������Խ�������Ȳ�����
    * Author       : 2018/5/15 ���ڶ�, by redmorningcn
    *******************************************************************************/
    for(i = 0;i < len;i++)                                       
    {
        if(((adr +i)&0x0FFF)==0)                                //�ڿ�߽磬��Ҫ������
        {
            
            EEP_WaitEvent();
            MX25L3206_Erase(((adr +i)>>12)&0x3FF,((adr +i)>>12)&0x3FF);
            
            i += 0x0FFF;
            EEP_SendEvent();
            //break;                                              
        }
    }
    
	return MX25L3206_WR(adr, buf, len);
}

/*******************************************************************************
* Description  : FLASH������
* Author       : 2018/5/15 ���ڶ�, by redmorningcn
*******************************************************************************/
void ReadFlsh(u32 adr,u8 *buf,u32 len)
{
	MX25L3206_RD(adr,len,buf);
}


