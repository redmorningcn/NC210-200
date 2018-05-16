#ifndef	SPRINTF_H
#define	SPRINTF_H
/*******************************************************************************
 *   Filename:       sprintf.h.h
 *   Revised:        Date: 2017/11/17 $
 *   Revision:       V1.0
 *   Writer:         Wuming Shen.
 *
 *   Description:
 *
 *   Notes:
 *                   QQ:276193028
 *                   E-mail:shenchangwei945@163.com
 *
 *   All copyrights reserved to Wuming Shen.
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#include <global.h>
#include <os.h>
#include <osal.h>
#include <mb.h>
     
#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef   APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
#endif
/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */
 
/*******************************************************************************
 * EXTERN FUNCTIONS
 */ 
void        uprintf                 (const char *fmt, ...);
int         usprintf                (char *buf, const char *fmt, ...);
void        uartprintf              (MODBUS_CH  *pch,const char *fmt, ...);

/*******************************************************************************
 * 				end of file                                                    *
 *******************************************************************************/
#ifdef __cplusplus
}
#endif
#endif	/* SPRINTF_H */
