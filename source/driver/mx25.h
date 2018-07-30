/*******************************************************************************
* Description  : FLASH��дdriver
* Author       : 2018/5/15 ���ڶ�, by redmorningcn
*******************************************************************************/
#ifndef  _MX25_H_
#define  _MX25_H_

#include    <includes.h>
//#define FLSH_MAX_SIZE   0x2000000             /* 32M     33554432byte  */
#define FLSH_MAX_SIZE   (0x1000000)             /* 16M     33554432byte  */
//#define MAX_ADDR		(32/8*1024*1024-256)	/* ����оƬ�ڲ�����ַ */
#define MAX_ADDR		(FLSH_MAX_SIZE)	        /* ����оƬ�ڲ�����ַ */
#define	SEC_MAX     	(4096)		            /* �������������      */
#define SEC_SIZE		(0x1000)                  /* ������С		4kByte */
#define SEC_LEFT_MOVE   (12)                    /* ������������λ��SEC_SIZE λ�� -1 ��*/

extern void SPI_FLASH_Init(void) ;
extern u32 MX25L3206_RdID(void);
extern u8 WriteFlsh(u32 adr,u8 *buf,u32 len);
extern void ReadFlsh(u32 adr,u8 *buf,u32 len);

extern u8 Send_Byte(u8 byte);
extern u8 Get_Byte(void);
extern void SPI_Flash_WAKEUP(void);
extern u8 MX25L3206_RD(u32 Dst, u32 NByte,u8* RcvBufPt);
extern u8 MX25L3206_WR(u32 Dst, u8* SndbufPt, u32 NByte);
extern u8 MX25L3206_Erase(u32 sec1, u32 sec2);

#endif

