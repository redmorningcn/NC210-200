/*******************************************************************************
 *   Filename:       osal_memory.h
 *   Revised:        $Date: 2013-04-17 $
 *   Revision:       $
 *   Writer:         Wuming Shen.
 *
 *   Description:
 *
 *   Notes:
 *
 *
 *   All copyrights reserved to Wuming Shen.
 *
 *******************************************************************************/
#ifndef OSAL_MEMORY_H
#define	OSAL_MEMORY_H

#ifdef	__cplusplus
extern "C" {
#endif
/*******************************************************************************
 * INCLUDES
 */
#include <osal.h>

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
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
#if OSAL_MEM_EN > 0
void        osal_mem_init       ( void );
void        osal_mem_kick       ( void );
void       *osal_mem_alloc      ( INT16U size );
void        osal_mem_free       ( void *ptr );
#endif
/*******************************************************************************
 * 				     end of file                               *
 *******************************************************************************/
#ifdef	__cplusplus
}
#endif

#endif	/* OSAL_MEMORY_H */
