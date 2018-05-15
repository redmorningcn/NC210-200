
/*******************************************************************************
 *   Filename:       bsp_time.h
 *   Revised:        $Date: 2014-05-27$
 *   Revision:       $
 *	 Writer:		 Wuming Shen.
 *
 *   Description:    
 *   Notes:        
 *					QQ:276193028                            			                  
 *     				E-mail:shenchangwei945@163.com    
 *			
 *   All copyrights reserved to Wuming Shen. 
 *
 *******************************************************************************/

#ifndef	BSP_TIME_H
#define	BSP_TIME_H
/*******************************************************************************
 * INCLUDES
// */
#include  <global.h>
#include  <time.h>
#include  <includes.h>

/*******************************************************************************
 * CONSTANTS
 */ 

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * TYPEDEFS
 */
typedef struct tm time_s;

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */
extern struct tm   tm_now;
extern time_t      t_now, t_last;

/*******************************************************************************
 * LOCAL FUNCTIONS
 */

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */
struct tm    TIME_ConvUnixToCalendar (time_t t);
time_t       TIME_ConvCalendarToUnix (struct tm t);
time_t       TIME_GetUnixTime        (void);
void         TIME_SetUnixTime        (time_t);
struct tm    TIME_GetCalendarTime    (void);
void         TIME_SetCalendarTime    (struct tm t);
BOOL         BSP_TIME_Init           (void);
/*******************************************************************************
 * EXTERN VARIABLES
 */ 

/*******************************************************************************
 * EXTERN FUNCTIONS
 */ 

/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/
#endif