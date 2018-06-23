/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                           (c) Copyright 2009-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                         Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : JJL
*                 EHS
*                 DC
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__


/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/

#define  APP_CFG_SERIAL_EN                          DEF_ENABLED
#define  APP_CFG_MODBUS_EN                          DEF_ENABLED                 /* uc/modbus ģ�� �������ֹ*/
#define  APP_CFG_OSAL_EN                            DEF_ENABLED                 /* OSAL ģ�� �������ֹ*/



/*
*********************************************************************************************************
*                                            TASK PRIORITIES���������ȼ�����֤�ĵ����࣬�������Ч������
*********************************************************************************************************
*/
#define  MB_OS_CFG_RX_TASK_PRIO     (OS_CFG_PRIO_MAX - 14)  /* ModBus��������ȼ���û���õ�ModBus����Ҫ��ע��   */
#define  APP_TASK_START_PRIO        (OS_CFG_PRIO_MAX - 13)  /* start���������ȼ�                                  */ 
#define  APP_TASK_COMM_PRIO         (OS_CFG_PRIO_MAX - 12 )  /* COM�������ȼ�                                     */

#define  APP_TASK_OSAL_PRIO         (OS_CFG_PRIO_MAX -  3)  /* OSAL�������ȼ�                                     */




/*
*********************************************************************************************************
*                                           ���������ջ�Ĵ�С
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*                      ��ջ���ֽڴ�С = CPU���ݿ�� * 8 * size = 4 * 8 * size(�ֽ�)
*                           �磺APP_TASK_SAMPLE_STK_SIZE = 128u��ʵ��Ϊ 128u * 4 = 512���ֽڣ�
*                             ������IAR�����в鿴��ջ��С�����Ƿ���㡣
*********************************************************************************************************
*/
#define  MB_OS_CFG_RX_TASK_STK_SIZE                 512u        //ModBus�����ջ�Ĵ�С
#define  APP_TASK_START_STK_SIZE                    128u        //����ʼ��������ջ
#define  APP_TASK_OSAL_STK_SIZE                     512u        //OSAL�����ջ
#define  APP_TASK_COMM_STK_SIZE                     512u        //COMM�����ջ




/*
*********************************************************************************************************
*                                    BSP CONFIGURATION: RS-232
*********************************************************************************************************
*/

#define  BSP_CFG_SER_COMM_SEL             			BSP_SER_COMM_UART_02
#define  BSP_CFG_TS_TMR_SEL                         2


/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/
#if 0
#define  TRACE_LEVEL_OFF                            0
#define  TRACE_LEVEL_INFO                           1
#define  TRACE_LEVEL_DEBUG                          2
#endif

#define  APP_TRACE_LEVEL                            TRACE_LEVEL_INFO
#define  APP_TRACE                                  BSP_Ser_Printf

#define  APP_TRACE_INFO(x)            ((APP_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_TRACE x) : (void)0)
#define  APP_TRACE_DEBUG(x)           ((APP_TRACE_LEVEL >= TRACE_LEVEL_DEBUG) ? (void)(APP_TRACE x) : (void)0)




/*
*********************************************************************************************************
*                                    APP
*********************************************************************************************************
*/
/*******************************************************************************
 * 				            OSAL��ض���                                       *
 *******************************************************************************/
/*******************************************************************************
* ������ OSAL�������ȼ�����
*/
#define	OS_TASK_PRO_TMR                 OS_TASK_PRIO_HIGHEST - 2
#define	OS_TASK_PRO_DISP                OS_TASK_PRIO_HIGHEST - 3
#define	OS_TASK_PRO_STORE               OS_TASK_PRIO_HIGHEST - 4
#define	OS_TASK_PRO_LED                 OS_TASK_PRIO_HIGHEST - 5


/***************************************************
* ������ OSAL����ID����
*/
#define OS_TASK_ID_TMR                  0x02
#define OS_TASK_ID_DISP                 0x03
#define OS_TASK_ID_STORE		        0x04
#define OS_TASK_ID_LED		            0x05


/*******************************************************************************
* ������ OSAL�¼�����
*/
/***************************************************
* ������ OSAL�¼����壺TMR�����¼�(20170930 ��ʱ���᲻ʱֹͣ���ͣ�����������8��)
*/
//#define OS_EVT_TMR_TICKS                0X00000001
//#define OS_EVT_TMR_MTR                  0X00000002
#define OS_EVT_TMR_MSEC                 0X00000004

/***************************************************
* ������ OSAL�¼����壺LED�����¼�
*/
#define OS_EVT_LED_TICKS                0X00000001
#define OS_EVT_LED_1                    0X00000002
#define OS_EVT_LED_2                    0X00000004
#define OS_EVT_LED_3                    0X00000008
#define OS_EVT_LED_4                    0X00000010
#define OS_EVT_LED_5                    0X00000020
#define OS_EVT_LED_6                    0X00000040
#define OS_EVT_LED_7                    0X00000080

/***************************************************
* ������ OSAL�¼����壺DISP�����¼�
*/
#define OS_EVT_DISP_TICKS               0X00000001
#define OS_EVT_DISP_ALARM               0X00000002
#define OS_EVT_DISP_2                   0X00000004
#define OS_EVT_DISP_3                   0X00000008
#define OS_EVT_DISP_4                   0X00000010
#define OS_EVT_DISP_5                   0X00000020
#define OS_EVT_DISP_6                   0X00000040
#define OS_EVT_DISP_7                   0X00000080

/***************************************************
* ������ OSAL�¼����壺STORE �����¼�
*/
#define OS_EVT_STORE_TICKS              0X00000001
#define OS_EVT_STORE_FRAM               0X00000002
#define OS_EVT_STORE_2                  0X00000004
#define OS_EVT_STORE_3                  0X00000008
#define OS_EVT_STORE_4                  0X00000010
#define OS_EVT_STORE_5                  0X00000020
#define OS_EVT_STORE_6                  0X00000040
#define OS_EVT_STORE_7                  0X00000080

/*******************************************************************************
* Description  : ���������¼���ʶ��
* Author       : 2018/5/18 ������, by redmorningcn
*/
#define COMM_EVT_FLAG_MTR_RX            (0x1 << 0)	/* MTR �����¼�                 */
#define COMM_EVT_FLAG_DTU_RX            (0x1 << 1)	/* DTU �����¼�                 */
#define COMM_EVT_FLAG_TAX_RX            (0x1 << 2)	/* TAX �����¼�                 */
#define COMM_EVT_FLAG_MTR_TX            (0x1 << 3)	/* MTR �����¼�                 */
#define COMM_EVT_FLAG_DTU_TX            (0x1 << 4)	/* DTU �����¼�                 */
#define COMM_EVT_FLAG_TAX_TX            (0x1 << 5)	/* TAX �����¼�                 */
#endif
