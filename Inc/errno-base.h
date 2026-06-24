#ifndef ERRNO_BASE_H
#define ERRNO_BASE_H

/** @defgroup error code definitions
  * @{
  */
#define	ERR_PERM                    1	/**< Operation not permitted */
#define	ERR_NOENT                   2	/**< No such file or directory */
#define	ERR_SRCH		            3	/**< No such process */
#define	ERR_INTR		            4	/**< Interrupted system call */
#define	ERR_IO		                5	/**< I/O error */
#define	ERR_NXIO		            6	/**< No such device or address */
#define	ERR_2BIG		            7	/**< Argument list too long */
#define	ERR_NOEXEC		            8	/**< Exec format error */
#define	ERR_BADF		            9	/**< Bad file number */
#define	ERR_CHILD		            10	/**< No child processes */
#define	ERR_AGAIN		            11	/**< Try again */
#define	ERR_NOMEM		            12	/**< Out of memory */
#define	ERR_ACCES		            13	/**< Permission denied */
#define	ERR_FAULT		            14	/**< Bad address */
#define	ERR_NOTBLK		            15	/**< Block device required */
#define	ERR_BUSY		            16	/**< Device or resource busy */
#define	ERR_EXIST		            17	/**< File exists */
#define	ERR_EXDEV		            18	/**< Cross-device link */
#define	ERR_NODEV		            19	/**< No such device */
#define	ERR_NOTDIR		            20	/**< Not a directory */
#define	ERR_ISDIR		            21	/**< Is a directory */
#define	ERR_INVAL		            22	/**< Invalid argument */
#define	ERR_ENFILE		            23	/**< File table overflow */
#define	ERR_EMFILE		            24	/**< Too many open files */
#define	ERR_NOTTY		            25	/**< Not a typewriter */
#define	ERR_TXTBSY		            26	/**< Text file busy */
#define	ERR_FBIG		            27	/**< File too large */
#define	ERR_ENOSPC		            28	/**< No space left on device */
#define	ERR_ESPIPE		            29	/**< Illegal seek */
#define	ERR_ROFS		            30	/**< Read-only file system */
#define	ERR_MLINK		            31	/**< Too many links */
#define	ERR_PIPE		            32	/**< Broken pipe */
#define	ERR_DOM		                33	/**< Math argument out of domain of func */
#define	ERR_RANGE		            34	/**< Math result not representable */
#define ERR_TIMEOUT                 35  /**< Time out */
#define ERR_NOSYS                   36  /**< Invalid system call number */
#define ERR_NOTSUPP                 37  /**< Operation not supported */
#define	ERR_NAMETOOLONG	            38	/**< File name too long */
#define ERR_UCLEAN                  117 /**< Structure needs cleaning (ECC correctable but over threshold) */
/**
  * @}
  */

#endif // ERRNO_BASE_H