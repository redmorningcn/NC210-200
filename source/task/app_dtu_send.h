/**************************************************************
* Description  : ������DTUͨѶ
* Author       : 2018/5/23 ������, by redmorningcn
*/
#ifndef  _APP_DTU_SEND_H_
#define  _APP_DTU_SEND_H_

#include <includes.h>

#define IAP_FRAME_CODE              0x1a      /* IAP ����֡������     */
#define SET_FRAME_CODE              0x12      /* SET ���ÿ�����       */
#define RECORD_FRAME_CODE           0x10      /* ���ݼ�¼csnc������    */     

#define SET_REPLY_DATA_LEN          (4+4+1)   /* ��ѯ�����ò�����Ӧ�����ݳ���*/
#define IAP_REPLY_DATA_LEN          (4)       /* IAP��������������֡Ӧ�𳤶� */

#endif

