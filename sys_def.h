/**
  ******************************************************************************
  * @file        : xxx.h
  * @author      : ZJY
  * @version     : V1.0
  * @date        : 20xx-xx-xx
  * @brief       : 
  * @attention   : None
  ******************************************************************************
  * @history     :
  *         V1.0 : 1.xxx
  ******************************************************************************
  */
#ifndef __SYS_DEF_H__
#define __SYS_DEF_H__

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "sys_config.h"
#include "types.h"

/* Exported define -----------------------------------------------------------*/
/** @defgroup error code definitions
  * @{
  */
#define	EPERM                   1	/**< Operation not permitted */
#define	ENOENT                  2	/**< No such file or directory */
#define	ESRCH		            3	/**< No such process */
#define	EINTR		            4	/**< Interrupted system call */
#define	EIO		                5	/**< I/O error */
#define	ENXIO		            6	/**< No such device or address */
#define	E2BIG		            7	/**< Argument list too long */
#define	ENOEXEC		            8	/**< Exec format error */
#define	EBADF		            9	/**< Bad file number */
#define	ECHILD		            10	/**< No child processes */
#define	EAGAIN		            11	/**< Try again */
#define	ENOMEM		            12	/**< Out of memory */
#define	EACCES		            13	/**< Permission denied */
#define	EFAULT		            14	/**< Bad address */
#define	ENOTBLK		            15	/**< Block device required */
#define	EBUSY		            16	/**< Device or resource busy */
#define	EEXIST		            17	/**< File exists */
#define	EXDEV		            18	/**< Cross-device link */
#define	ENODEV		            19	/**< No such device */
#define	ENOTDIR		            20	/**< Not a directory */
#define	EISDIR		            21	/**< Is a directory */
#define	EINVAL		            22	/**< Invalid argument */
#define	ENFILE		            23	/**< File table overflow */
#define	EMFILE		            24	/**< Too many open files */
#define	ENOTTY		            25	/**< Not a typewriter */
#define	ETXTBSY		            26	/**< Text file busy */
#define	EFBIG		            27	/**< File too large */
#define	ENOSPC		            28	/**< No space left on device */
#define	ESPIPE		            29	/**< Illegal seek */
#define	EROFS		            30	/**< Read-only file system */
#define	EMLINK		            31	/**< Too many links */
#define	EPIPE		            32	/**< Broken pipe */
#define	EDOM		            33	/**< Math argument out of domain of func */
#define	ERANGE		            34	/**< Math result not representable */
#define ETIMEOUT                35  /**< Time out */
#define ENOSYS                  36  /**< Invalid system call number */
#define ENOTSUPP                37  
#define	ENAMETOOLONG	        38	/**< File name too long */
/**
  * @}
  */
/* Exported typedef ----------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/**
 * @brief return the member address of ptr, if the type of ptr is the struct type.
 */
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

/* Exported variable prototypes ----------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SYS_DEF_H__ */

