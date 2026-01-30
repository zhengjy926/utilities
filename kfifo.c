/**
 *******************************************************************************
 * @file        : kfifo.c
 * @author      : ZJY
 * @version     : V1.0
 * @date        : 2024-07-16
 * @brief       : Kernel FIFO implementation
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "kfifo.h"
#include "errno-base.h"
#include "math/log2.h"
#include "math/minmax.h"
#include <string.h>

/* Private function prototypes -----------------------------------------------*/
static inline unsigned int kfifo_unused(const kfifo_t *fifo);
static void kfifo_copy_in(kfifo_t *fifo,
                          const void *src,
                          unsigned int len,
                          unsigned int off);
static void kfifo_copy_out(kfifo_t *fifo,
                           void *dst,
                           unsigned int len,
                           unsigned int off);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize a FIFO buffer
 * @param fifo   Pointer to the FIFO structure to initialize
 * @param buffer Pointer to the storage buffer
 * @param size   Size of the storage buffer in bytes
 * @param esize  Size of each element in bytes
 * @retval 0       Success
 * @retval -EINVAL Invalid parameters
 */
int kfifo_init(kfifo_t *fifo,
               void *buffer,
               unsigned int size,
               unsigned int esize)
{
	size /= esize;

	if (!is_power_of_2(size))
		size = rounddown_pow_of_two(size);

	fifo->in = 0;
	fifo->out = 0;
	fifo->esize = esize;
	fifo->data = buffer;

	if (size < 2) {
		fifo->mask = 0;
		return -EINVAL;
	}
	fifo->mask = size - 1;

    return 0;
}

/**
 * @brief Put data into the FIFO
 * @param fifo Pointer to the FIFO structure
 * @param buf  Pointer to the data to be added (elements)
 * @param len  Number of elements to be added
 * @return Number of elements actually copied
 */
unsigned int kfifo_in(kfifo_t *fifo, const void *buf, unsigned int len)
{
	unsigned int l;

	l = kfifo_unused(fifo);
	if (len > l)
		len = l;

	kfifo_copy_in(fifo, buf, len, fifo->in);
	fifo->in += len;
	return len;
}

/**
 * @brief Put data into the FIFO with interrupt protection
 * @param fifo Pointer to the FIFO structure
 * @param buf  Pointer to the data to be added (elements)
 * @param len  Number of elements to be added
 * @return Number of elements actually copied
 */
unsigned int kfifo_in_locked(kfifo_t *fifo, const void *buf, unsigned int len)
{
    unsigned int ret;
    uint32_t primask;

    primask = __get_PRIMASK();
    __disable_irq();
    ret = kfifo_in(fifo, buf, len);
    __set_PRIMASK(primask);

    return ret;
}

/**
 * @brief Peek data from the FIFO without removing it
 * @param fifo Pointer to the FIFO structure
 * @param buf  Pointer to the storage buffer (elements)
 * @param len  Number of elements to peek
 * @return Number of elements actually copied
 */
unsigned int kfifo_out_peek(kfifo_t *fifo, void *buf, unsigned int len)
{
	unsigned int l;

	l = fifo->in - fifo->out;
	if (len > l)
		len = l;

	kfifo_copy_out(fifo, buf, len, fifo->out);
	return len;
}

/**
 * @brief Get the length of the linear space that can be read
 * @param fifo Pointer to the FIFO structure
 * @param tail Pointer to return the starting position that can be read
 *             in the linear space (element index). Can be NULL.
 * @param n    Number of elements requested to be read
 * @return Available count till the end of data or till the end of the buffer
 * @note With only one concurrent reader and one concurrent writer,
 *       you don't need extra locking to use this function.
 */
unsigned int kfifo_out_linear(kfifo_t *fifo,
                              unsigned int *tail,
                              unsigned int n)
{
	unsigned int size = fifo->mask + 1;
	unsigned int off = fifo->out & fifo->mask;

	if (tail)
		*tail = off;

	return min3(n, fifo->in - fifo->out, size - off);
}

/**
 * @brief Get data from the FIFO
 * @param fifo Pointer to the FIFO structure
 * @param buf  Pointer to the storage buffer (elements)
 * @param len  Number of elements to get
 * @return Number of elements actually copied
 */
unsigned int kfifo_out(kfifo_t *fifo, void *buf, unsigned int len)
{
	len = kfifo_out_peek(fifo, buf, len);
	fifo->out += len;
	return len;
}

/**
 * @brief Get data from the FIFO with interrupt protection
 * @param fifo Pointer to the FIFO structure
 * @param buf  Pointer to the storage buffer (elements)
 * @param len  Number of elements to get
 * @return Number of elements actually copied
 */
unsigned int kfifo_out_locked(kfifo_t *fifo, void *buf, unsigned int len)
{
    unsigned int ret;
    uint32_t primask;

    primask = __get_PRIMASK();
    __disable_irq();
    ret = kfifo_out(fifo, buf, len);
    __set_PRIMASK(primask);

    return ret;
}

/**
 * @brief Get the length of the linear space that can be read with interrupt protection
 * @param fifo Pointer to the FIFO structure
 * @param tail Pointer to return the starting position that can be read
 *             in the linear space (element index). Can be NULL.
 * @param n    Number of elements requested to be read
 * @return Available count till the end of data or till the end of the buffer
 */
unsigned int kfifo_out_linear_locked(kfifo_t *fifo,
                                     unsigned int *tail,
                                     unsigned int n)
{
    unsigned int ret;
    uint32_t primask;

    primask = __get_PRIMASK();
    __disable_irq();
    ret = kfifo_out_linear(fifo, tail, n);
    __set_PRIMASK(primask);

    return ret;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Get the unused space in the FIFO
 * @param fifo Pointer to the FIFO structure
 * @return Number of unused elements in the FIFO
 */
static inline unsigned int kfifo_unused(const kfifo_t *fifo)
{
	return (fifo->mask + 1) - (fifo->in - fifo->out);
}

/**
 * @brief Copy data into the FIFO buffer
 * @param fifo Pointer to the FIFO structure
 * @param src  Pointer to the source buffer (elements)
 * @param len  Number of elements to be copied
 * @param off  Offset in the destination buffer (elements)
 * @details This function handles circular buffer wraparound and ensures
 *          data consistency with memory barriers.
 */
static void kfifo_copy_in(kfifo_t *fifo,
                          const void *src,
                          unsigned int len,
                          unsigned int off)
{
	unsigned int size = fifo->mask + 1;
	unsigned int esize = fifo->esize;
	unsigned int l;

	off &= fifo->mask;
	if (esize != 1) {
		off *= esize;
		size *= esize;
		len *= esize;
	}
	l = min(len, size - off);

	memcpy(fifo->data + off, src, l);
	memcpy(fifo->data, src + l, len - l);

    /* make sure that the data in the fifo is up to date before
     * incrementing the fifo->in index counter */
    __DMB();
}

/**
 * @brief Copy data from the FIFO buffer
 * @param fifo Pointer to the FIFO structure
 * @param dst  Pointer to the destination buffer (elements)
 * @param len  Number of elements to be copied
 * @param off  Offset in the source buffer (elements)
 * @details This function handles circular buffer wraparound and ensures
 *          data consistency with memory barriers.
 */
static void kfifo_copy_out(kfifo_t *fifo,
                           void *dst,
                           unsigned int len,
                           unsigned int off)
{
	unsigned int size = fifo->mask + 1;
	unsigned int esize = fifo->esize;
	unsigned int l;

	off &= fifo->mask;
	if (esize != 1) {
		off *= esize;
		size *= esize;
		len *= esize;
	}
	l = min(len, size - off);

	memcpy(dst, fifo->data + off, l);
	memcpy(dst + l, fifo->data, len - l);

    /* make sure that the data is copied before
     * incrementing the fifo->out index counter */
    __DMB();
}

/**************************** End of file *************************************/
