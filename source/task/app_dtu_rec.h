/**************************************************************
* Description  : ������DTUͨѶ�����ݽ��ղ���
* Author       : 2018/5/24 ������, by redmorningcn
*/

#ifndef  _APP_DTU_REC_H_
#define  _APP_DTU_REC_H_


#include <includes.h>
#include <app_dtu_send.h>

#define		CMD_TIME_SET        0x00000100			//ʱ�俨��IC/���ߣ�
#define		CMD_LOCO_SET        0x00002000			//װ������IC/���ߣ�
#define		CMD_FRAM_COPY	    0x00004000			//���翽������IC��
#define		CMD_REC_CLR			0x00080000			//��¼���㣨���ߣ�
#define		CMD_SYS_RST	        0x00100000		    //ϵͳ��λ�����ߣ�
#define		CMD_PARA_SET        0x00800000		    //����д���������ߣ�
#define		CMD_PARA_GET        0x00800001		    //���ڶ����������ߣ�


#endif

