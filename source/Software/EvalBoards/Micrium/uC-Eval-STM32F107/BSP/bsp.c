/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                        Evaluation Board
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : EHS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_MODULE
#include <bsp.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/

#define  BSP_LED_START_BIT  (13 - 1)                            /* LEDs[3:1] are sequentially connected to PTD[15:13].  */


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq_MHz;


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  BSP_LED_Init   (void);
static  void  BSP_StatusInit (void);

/*
*********************************************************************************************************
*                                             REGISTERS
*********************************************************************************************************
*/

#define  DWT_CR      *(CPU_REG32 *)0xE0001000
#define  DWT_CYCCNT  *(CPU_REG32 *)0xE0001004
#define  DEM_CR      *(CPU_REG32 *)0xE000EDFC
#define  DBGMCU_CR   *(CPU_REG32 *)0xE0042004


/*
*********************************************************************************************************
*                                            REGISTER BITS
*********************************************************************************************************
*/

#define  DBGMCU_CR_TRACE_IOEN_MASK       0x10
#define  DBGMCU_CR_TRACE_MODE_ASYNC      0x00
#define  DBGMCU_CR_TRACE_MODE_SYNC_01    0x40
#define  DBGMCU_CR_TRACE_MODE_SYNC_02    0x80
#define  DBGMCU_CR_TRACE_MODE_SYNC_04    0xC0
#define  DBGMCU_CR_TRACE_MODE_MASK       0xC0

#define  DEM_CR_TRCENA                   (1 << 24)

#define  DWT_CR_CYCCNTENA                (1 <<  0)


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/
/*
#if ((CPU_CFG_TS_TMR_EN          != DEF_ENABLED) && \
     (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && \
     (OS_PROBE_HOOKS_EN          >  0u))
#error  "CPU_CFG_TS_EN                  illegally #define'd in 'cpu.h'"
#error  "                              [MUST be  DEF_ENABLED] when    "
#error  "                               using uC/Probe COM modules    "
#endif

*/

/*******************************************************************************
* Description  : 快速配置GPIO口
* Author       : 2018/5/15 星期二, by redmorningcn
*******************************************************************************/
void	GPIO_Config(u16 GpioNum,u8 FuncNum)
{
    GPIO_InitTypeDef  gpio_init;
    u8		PortNum;
	u8		IoNum;
    u32 RccBuf[] = {    RCC_APB2Periph_GPIOA,
                        RCC_APB2Periph_GPIOB,
                        RCC_APB2Periph_GPIOC,
                        RCC_APB2Periph_GPIOD,
                        RCC_APB2Periph_GPIOE,
                        RCC_APB2Periph_GPIOF,
                        RCC_APB2Periph_GPIOG
                    };
    
    GPIO_TypeDef* PortBuf[] = {   GPIOA,
                        GPIOB,
                        GPIOC,
                        GPIOD,
                        GPIOE,
                        GPIOF,
                        GPIOG
                    };
    
    PortNum 	= GpioNum/100;
	IoNum	    = GpioNum%100;
    
    RCC_APB2PeriphClockCmd(RccBuf[PortNum], ENABLE);

    gpio_init.GPIO_Pin   = (0x01<<IoNum);
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    
    if(FuncNum == 0)
        gpio_init.GPIO_Mode  = GPIO_Mode_Out_PP;        //推拉输出
    else
        gpio_init.GPIO_Mode  = GPIO_Mode_IPU;           //上拉输入

    GPIO_Init(PortBuf[PortNum], &gpio_init);
}

/*******************************************************************************
* Description  : GPIO端口设值（pb01=101）
* Author       : 2018/5/15 星期二, by redmorningcn
*******************************************************************************/
void	GPIO_Set(u32 GpioNum,u8 Val)
{
    GPIO_TypeDef* PortBuf[] = {   GPIOA,
                        GPIOB,
                        GPIOC,
                        GPIOD,
                        GPIOE,
                        GPIOF,
                        GPIOG
                    };
	u8		PortNum;
	u8		IoNum;

	PortNum 	= GpioNum/100;
	IoNum	    = GpioNum%100;
    
    GPIO_Config(GpioNum,0);                           //将端口置为输出
    
    if(Val)
        GPIO_SetBits(  PortBuf[PortNum],(0x01<<IoNum));     //将端口置1 
    else
        GPIO_ResetBits(PortBuf[PortNum],(0x01<<IoNum));     //将端口置0
}

/*******************************************************************************
* Description  : 返回IO端口值
* Author       : 2018/5/15 星期二, by redmorningcn
*******************************************************************************/
u8	GPIO_Read(u32 GpioNum)
{
    GPIO_TypeDef* PortBuf[] = {   GPIOA,
                        GPIOB,
                        GPIOC,
                        GPIOD,
                        GPIOE,
                        GPIOF,
                        GPIOG
                    };
	u8		PortNum;
	u8		IoNum;

	PortNum 	= GpioNum/100;
	IoNum	    = GpioNum%100;
    
    GPIO_Config(GpioNum,1);                           //将端口置为输入

    return GPIO_ReadInputDataBit(PortBuf[PortNum], (0x01<<IoNum));
}

/*******************************************************************************
* Description  : GPIO端口设值（pb01=101）
* Author       : 2018/5/15 星期二, by redmorningcn
*******************************************************************************/
void	GPIO_Toggle(u32 GpioNum)
{
    GPIO_TypeDef* PortBuf[] = {   
                        GPIOA,
                        GPIOB,
                        GPIOC,
                        GPIOD,
                        GPIOE,
                        GPIOF,
                        GPIOG
                    };
	u8		PortNum;
	u8		IoNum;
    
    PortNum 	= GpioNum/100;
	IoNum	    = GpioNum%100;
    
    GPIO_Config(GpioNum,0);                     //将端口置为输出
    
    if(GPIO_ReadOutputDataBit(PortBuf[PortNum], (0x01<<IoNum)))
        GPIO_Set(GpioNum,0);                    //如果端口为1，则将端口该写为0
    else
        GPIO_Set(GpioNum,1);
}


/*
*********************************************************************************************************
*                                               BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*
*               (2) CPU instruction / data tracing requires the use of the following pins :
*                   (a) (1) Aysynchronous     :  PB[3]
*                       (2) Synchronous 1-bit :  PE[3:2]
*                       (3) Synchronous 2-bit :  PE[4:2]
*                       (4) Synchronous 4-bit :  PE[6:2]
*
*                   (b) The uC-Eval board MAY utilize the following pins depending on the application :
*                       (1) PE[5], MII_INT
*                       (1) PE[6], SDCard_Detection
*
*                   (c) The application may wish to adjust the trace bus width depending on I/O
*                       requirements.
*********************************************************************************************************
*/
void  BSP_Init (void)
{
    BSP_IntInit();

    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);                                  /* HSE = 25MHz ext. crystal.                            */
    RCC_WaitForHSEStartUp();


    RCC_PREDIV2Config(RCC_PREDIV2_Div5);                        /* Fprediv2 = HSE      /  5 =  5MHz.                    */
    RCC_PLL2Config(RCC_PLL2Mul_8);                              /* PLL2     = Fprediv2 *  8 = 40MHz.                    */
    RCC_PLL2Cmd(ENABLE);
    RCC_PLL3Config(RCC_PLL3Mul_10);                             /* PLL3     = Fprediv2 * 10 = 50MHz.                    */
    RCC_PLL3Cmd(ENABLE);

    RCC_HCLKConfig(RCC_SYSCLK_Div1);                            /* HCLK    = AHBCLK  = PLL1 / AHBPRES(1) = 72MHz.       */
    RCC_PCLK2Config(RCC_HCLK_Div1);                             /* APB2CLK = AHBCLK  / APB2DIV(1)  = 72MHz.             */
    RCC_PCLK1Config(RCC_HCLK_Div2);                             /* APB1CLK = AHBCLK  / APB1DIV(2)  = 36MHz (max).       */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);                           /* ADCCLK  = AHBCLK  / APB2DIV / 6 = 12MHz.             */
    RCC_OTGFSCLKConfig(RCC_OTGFSCLKSource_PLL1VCO_Div3);        /* OTGCLK  = PLL1VCO / USBPRES(3)  = 144MHz / 3 = 48MHz */

    FLASH_SetLatency(FLASH_Latency_2);                          /* 2 Flash wait states when HCLK > 48MHz.               */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    int time = 10000;       //redmorningcn 180530 防止卡死              
    while (RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET) {      /* Wait for PLL2 to lock.                               */
        time--;
        if(time == 0)
            break;
    }

    time = 10000;           //redmorningcn 180530 防止卡死              
    while (RCC_GetFlagStatus(RCC_FLAG_PLL3RDY) == RESET) {      /* Wait for PLL3 to lock.                               */
        time--;
        if(time == 0)
            break;
    }
                                                                /* Fprediv1 = PLL2 / 5 =  8MHz.                         */
    RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div5);
    RCC_PLL1Config(RCC_PLL1Source_PREDIV1, RCC_PLL1Mul_9);      /* PLL1 = Fprediv1 * 9 = 72Mhz.                         */
    RCC_PLL1Cmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_PLL1RDY) == RESET) {      /* Wait for PLL1 to lock.                               */
        ;
    }


    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLL1CLK);                 /* HCLK = SYSCLK = PLL1 = 72MHz.                        */
    while (RCC_GetSYSCLKSource() != 0x08) {
        ;
    }

    BSP_CPU_ClkFreq_MHz = BSP_CPU_ClkFreq() / (CPU_INT32U)1000000;

    BSP_CPU_ClkFreq_MHz = BSP_CPU_ClkFreq_MHz;                  /* Surpress compiler warning BSP_CPU_ClkFreq_MHz    ... */
                                                                /* ... set and not used.                                */

    BSP_LED_Init();                                             /* Initialize the I/Os for the LED      controls.       */

    BSP_StatusInit();                                           /* Initialize the status input(s)                       */

    extern  void    BSP_Init_Hook(void);
    BSP_Init_Hook();                                            //redmorningcn  BSP初始化钩子函数，在任务开始前执行 
    
#ifdef TRACE_EN                                                 /* See project / compiler preprocessor options.         */
    DBGMCU_CR |=  DBGMCU_CR_TRACE_IOEN_MASK;                    /* Enable tracing (see Note #2).                        */
    DBGMCU_CR &= ~DBGMCU_CR_TRACE_MODE_MASK;                    /* Clr trace mode sel bits.                             */
    DBGMCU_CR |=  DBGMCU_CR_TRACE_MODE_SYNC_04;                 /* Cfg trace mode to synch 4-bit.                       */
#endif
}


/*
*********************************************************************************************************
*                                            BSP_CPU_ClkFreq()
*
* Description : Read CPU registers to determine the CPU clock frequency of the chip.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    RCC_ClocksTypeDef  rcc_clocks;


    RCC_GetClocksFreq(&rcc_clocks);

    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                              LED FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             BSP_LED_Init()
*
* Description : Initialize the I/O for the LEDs
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_LED_Init (void)
{
    GPIO_Config(BSP_GPIO_LED0,0);           //端口配置为输出
    GPIO_Config(BSP_GPIO_LED1,0);           //端口配置为输出
    GPIO_Config(BSP_GPIO_LED2,0);           //端口配置为输出
    GPIO_Config(BSP_GPIO_LED3,0);           //端口配置为输出
    GPIO_Config(BSP_GPIO_LED4,0);           //端口配置为输出
    GPIO_Config(BSP_GPIO_LED5,0);           //端口配置为输出
    GPIO_Config(BSP_GPIO_LED6,0);           //端口配置为输出
    GPIO_Config(BSP_GPIO_LED7,0);           //端口配置为输出
}


/*
*********************************************************************************************************
*                                             BSP_LED_On()
*
* Description : Turn ON any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    turns ON ALL the LEDs
*                       1    turns ON user LED1  on the board
*                       2    turns ON user LED2  on the board
*                       3    turns ON user LED3  on the board
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_On (CPU_INT08U led)
{
    switch (led) {
        case 0:
                GPIO_Set(BSP_GPIO_LED0,0);                  //端口置ON
                GPIO_Set(BSP_GPIO_LED1,0);                  //端口置ON
                GPIO_Set(BSP_GPIO_LED2,0);                  //端口置ON
                GPIO_Set(BSP_GPIO_LED3,0);                  //端口置ON
                GPIO_Set(BSP_GPIO_LED4,0);                  //端口置ON
                GPIO_Set(BSP_GPIO_LED5,0);                  //端口置ON
                GPIO_Set(BSP_GPIO_LED6,0);                  //端口置ON
                GPIO_Set(BSP_GPIO_LED7,0);                  //端口置ON
             break;

        case 1:
                GPIO_Set(BSP_GPIO_LED0,0);                  //端口置ON
             break;

        case 2:
                GPIO_Set(BSP_GPIO_LED1,0);                  //端口置ON
             break;

        case 3:
                GPIO_Set(BSP_GPIO_LED2,0);                  //端口置ON
             break;
             
        case 4:
                GPIO_Set(BSP_GPIO_LED3,0);                  //端口置ON
             break;     
             
        case 5:
                GPIO_Set(BSP_GPIO_LED4,0);                  //端口置ON
             break;   
             
        case 6:
                GPIO_Set(BSP_GPIO_LED5,0);                  //端口置ON
             break;
             
        case 7:
                GPIO_Set(BSP_GPIO_LED6,0);                  //端口置ON
             break;
        case 8:
                GPIO_Set(BSP_GPIO_LED7,0);                  //端口置ON
             break;
             
        default:
             break;
    }
}


/*
*********************************************************************************************************
*                                              BSP_LED_Off()
*
* Description : Turn OFF any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    turns OFF ALL the LEDs
*                       1    turns OFF user LED1  on the board
*                       2    turns OFF user LED2  on the board
*                       3    turns OFF user LED3  on the board
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void  BSP_LED_Off (CPU_INT08U led)
{
    switch (led) {
        case 0:
                GPIO_Set(BSP_GPIO_LED0,1);                  //端口置OFF
                GPIO_Set(BSP_GPIO_LED1,1);                  //端口置OFF
                GPIO_Set(BSP_GPIO_LED2,1);                  //端口置OFF
                GPIO_Set(BSP_GPIO_LED3,1);                  //端口置OFF
                GPIO_Set(BSP_GPIO_LED4,1);                  //端口置OFF
                GPIO_Set(BSP_GPIO_LED5,1);                  //端口置OFF
                GPIO_Set(BSP_GPIO_LED6,1);                  //端口置OFF
                GPIO_Set(BSP_GPIO_LED7,1);                  //端口置OFF
             break;

        case 1:
                GPIO_Set(BSP_GPIO_LED0,1);                  //端口置OFF
             break;

        case 2:
                GPIO_Set(BSP_GPIO_LED1,1);                  //端口置OFF
             break;

        case 3:
                GPIO_Set(BSP_GPIO_LED2,1);                  //端口置OFF
             break;
             
        case 4:
                GPIO_Set(BSP_GPIO_LED3,1);                  //端口置OFF
             break;     
             
        case 5:
                GPIO_Set(BSP_GPIO_LED4,1);                  //端口置OFF
             break;   
             
        case 6:
                GPIO_Set(BSP_GPIO_LED5,1);                  //端口置OFF
             break;
             
        case 7:
                GPIO_Set(BSP_GPIO_LED6,1);                  //端口置OFF
             break;
        case 8:
                GPIO_Set(BSP_GPIO_LED7,1);                  //端口置OFF
             break;
             
        default:
             break;
    }
}


/*
*********************************************************************************************************
*                                            BSP_LED_Toggle()
*
* Description : TOGGLE any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    TOGGLE ALL the LEDs
*                       1    TOGGLE user LED1  on the board
*                       2    TOGGLE user LED2  on the board
*                       3    TOGGLE user LED3  on the board
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Toggle (CPU_INT08U led)
{
    switch (led) {
        case 0:
                GPIO_Toggle(BSP_GPIO_LED0);                  //端口置0
                GPIO_Toggle(BSP_GPIO_LED1);                  //端口置0
                GPIO_Toggle(BSP_GPIO_LED2);                  //端口置0
                GPIO_Toggle(BSP_GPIO_LED3);                  //端口置0
                GPIO_Toggle(BSP_GPIO_LED4);                  //端口置0
                GPIO_Toggle(BSP_GPIO_LED5);                  //端口置0
                GPIO_Toggle(BSP_GPIO_LED6);                  //端口置0
                GPIO_Toggle(BSP_GPIO_LED7);                  //端口置0
             break;

        case 1:
                GPIO_Toggle(BSP_GPIO_LED0);                  //端口置0
             break;

        case 2:
                GPIO_Toggle(BSP_GPIO_LED1);                  //端口置0
             break;

        case 3:
                GPIO_Toggle(BSP_GPIO_LED2);                  //端口置0
             break;
             
        case 4:
                GPIO_Toggle(BSP_GPIO_LED3);                  //端口置0
             break;     
             
        case 5:
                GPIO_Toggle(BSP_GPIO_LED4);                  //端口置0
             break;   
             
        case 6:
                GPIO_Toggle(BSP_GPIO_LED5);                  //端口置0
             break;
             
        case 7:
                GPIO_Toggle(BSP_GPIO_LED6);                  //端口置0
             break;
        case 8:
                GPIO_Toggle(BSP_GPIO_LED7);                  //端口置0
             break;
             
        default:
             break;
    }
}

/*
*********************************************************************************************************
*                                            BSP_StatusInit()
*
* Description : Initialize the status port(s)
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init()
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  BSP_StatusInit (void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;             /* PB5 is used to read the status of the LM75 */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*                                            BSP_StatusRd()
*
* Description : Get the current status of a status input
*
* Argument(s) : id    is the status you want to get.
*
* Return(s)   : DEF_ON    if the status is asserted
*               DEF_OFF   if the status is negated
*
* Caller(s)   : application
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_StatusRd (CPU_INT08U  id)
{
    CPU_BOOLEAN  bit_val;


    switch (id) {
        case 1:
             bit_val = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5);
             return (bit_val);

        default:
             return ((CPU_BOOLEAN)DEF_OFF);
    }
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           OS PROBE FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           OSProbe_TmrInit()
*
* Description : Select & initialize a timer for use with the uC/Probe Plug-In for uC/OS-II.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : OSProbe_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if ((APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && \
     (OS_PROBE_HOOKS_EN          == 1))
void  OSProbe_TmrInit (void)
{
}
#endif


/*
*********************************************************************************************************
*                                            OSProbe_TmrRd()
*
* Description : Read the current counts of a free running timer.
*
* Argument(s) : none.
*
* Return(s)   : The 32-bit timer counts.
*
* Caller(s)   : OSProbe_TimeGetCycles().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if ((APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && \
     (OS_PROBE_HOOKS_EN          == 1))
CPU_INT32U  OSProbe_TmrRd (void)
{
    return ((CPU_INT32U)DWT_CYCCNT);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                          CPU_TS_TmrInit()
*
* Description : Initialize & start CPU timestamp timer.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : CPU_TS_Init().
*
*               This function is an INTERNAL CPU module function & MUST be implemented by application/
*               BSP function(s) [see Note #1] but MUST NOT be called by application function(s).
*
* Note(s)     : (1) CPU_TS_TmrInit() is an application/BSP function that MUST be defined by the developer
*                   if either of the following CPU features is enabled :
*
*                   (a) CPU timestamps
*                   (b) CPU interrupts disabled time measurements
*
*                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
*                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
*
*               (2) (a) Timer count values MUST be returned via word-size-configurable 'CPU_TS_TMR'
*                       data type.
*
*                       (1) If timer has more bits, truncate timer values' higher-order bits greater
*                           than the configured 'CPU_TS_TMR' timestamp timer data type word size.
*
*                       (2) Since the timer MUST NOT have less bits than the configured 'CPU_TS_TMR'
*                           timestamp timer data type word size; 'CPU_CFG_TS_TMR_SIZE' MUST be
*                           configured so that ALL bits in 'CPU_TS_TMR' data type are significant.
*
*                           In other words, if timer size is not a binary-multiple of 8-bit octets
*                           (e.g. 20-bits or even 24-bits), then the next lower, binary-multiple
*                           octet word size SHOULD be configured (e.g. to 16-bits).  However, the
*                           minimum supported word size for CPU timestamp timers is 8-bits.
*
*                       See also 'cpu_cfg.h   CPU TIMESTAMP CONFIGURATION  Note #2'
*                              & 'cpu_core.h  CPU TIMESTAMP DATA TYPES     Note #1'.
*
*                   (b) Timer SHOULD be an 'up'  counter whose values increase with each time count.
*
*                   (c) When applicable, timer period SHOULD be less than the typical measured time
*                       but MUST be less than the maximum measured time; otherwise, timer resolution
*                       inadequate to measure desired times.
*
*                   See also 'CPU_TS_TmrRd()  Note #2'.
*********************************************************************************************************
*/

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void  CPU_TS_TmrInit (void)
{
    CPU_INT32U  cpu_clk_freq_hz;


    DEM_CR         |= (CPU_INT32U)DEM_CR_TRCENA;                /* Enable Cortex-M3's DWT CYCCNT reg.                   */
    DWT_CYCCNT      = (CPU_INT32U)0u;
    DWT_CR         |= (CPU_INT32U)DWT_CR_CYCCNTENA;

    cpu_clk_freq_hz = BSP_CPU_ClkFreq();
    CPU_TS_TmrFreqSet(cpu_clk_freq_hz);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                           CPU_TS_TmrRd()
*
* Description : Get current CPU timestamp timer count value.
*
* Argument(s) : none.
*
* Return(s)   : Timestamp timer count (see Notes #2a & #2b).
*
* Caller(s)   : CPU_TS_Init(),
*               CPU_TS_Get32(),
*               CPU_TS_Get64(),
*               CPU_IntDisMeasStart(),
*               CPU_IntDisMeasStop().
*
*               This function is an INTERNAL CPU module function & MUST be implemented by application/
*               BSP function(s) [see Note #1] but SHOULD NOT be called by application function(s).
*
* Note(s)     : (1) CPU_TS_TmrRd() is an application/BSP function that MUST be defined by the developer
*                   if either of the following CPU features is enabled :
*
*                   (a) CPU timestamps
*                   (b) CPU interrupts disabled time measurements
*
*                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
*                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
*
*               (2) (a) Timer count values MUST be returned via word-size-configurable 'CPU_TS_TMR'
*                       data type.
*
*                       (1) If timer has more bits, truncate timer values' higher-order bits greater
*                           than the configured 'CPU_TS_TMR' timestamp timer data type word size.
*
*                       (2) Since the timer MUST NOT have less bits than the configured 'CPU_TS_TMR'
*                           timestamp timer data type word size; 'CPU_CFG_TS_TMR_SIZE' MUST be
*                           configured so that ALL bits in 'CPU_TS_TMR' data type are significant.
*
*                           In other words, if timer size is not a binary-multiple of 8-bit octets
*                           (e.g. 20-bits or even 24-bits), then the next lower, binary-multiple
*                           octet word size SHOULD be configured (e.g. to 16-bits).  However, the
*                           minimum supported word size for CPU timestamp timers is 8-bits.
*
*                       See also 'cpu_cfg.h   CPU TIMESTAMP CONFIGURATION  Note #2'
*                              & 'cpu_core.h  CPU TIMESTAMP DATA TYPES     Note #1'.
*
*                   (b) Timer SHOULD be an 'up'  counter whose values increase with each time count.
*
*                       (1) If timer is a 'down' counter whose values decrease with each time count,
*                           then the returned timer value MUST be ones-complemented.
*
*                   (c) (1) When applicable, the amount of time measured by CPU timestamps is
*                           calculated by either of the following equations :
*
*                           (A) Time measured  =  Number timer counts  *  Timer period
*
*                                   where
*
*                                       Number timer counts     Number of timer counts measured
*                                       Timer period            Timer's period in some units of
*                                                                   (fractional) seconds
*                                       Time measured           Amount of time measured, in same
*                                                                   units of (fractional) seconds
*                                                                   as the Timer period
*
*                                                  Number timer counts
*                           (B) Time measured  =  ---------------------
*                                                    Timer frequency
*
*                                   where
*
*                                       Number timer counts     Number of timer counts measured
*                                       Timer frequency         Timer's frequency in some units
*                                                                   of counts per second
*                                       Time measured           Amount of time measured, in seconds
*
*                       (2) Timer period SHOULD be less than the typical measured time but MUST be less
*                           than the maximum measured time; otherwise, timer resolution inadequate to
*                           measure desired times.
*********************************************************************************************************
*/

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_TS_TMR  CPU_TS_TmrRd (void)
{
    return ((CPU_TS_TMR)DWT_CYCCNT);
}
#endif
