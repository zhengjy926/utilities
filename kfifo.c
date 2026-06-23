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
#include "Inc/kfifo.h"
#include "Inc/errno-base.h"
#include "Inc/log2.h"
#include "Inc/minmax.h"
#include <string.h>

/* Private function prototypes -----------------------------------------------*/
static inline uint32_t Kfifo_GetUnused(const Kfifo_t *fifo);
static void Kfifo_CopyIn(Kfifo_t *fifo, const void *src, uint32_t len, uint32_t off);
static void Kfifo_CopyOut(Kfifo_t *fifo, void *dst, uint32_t len, uint32_t off);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize a FIFO buffer
 * @param fifo   Pointer to the FIFO structure to initialize
 * @param buffer Pointer to the storage buffer
 * @param size   Size of the storage buffer in bytes
 * @param esize  Size of each element in bytes
 * @retval 0       Success
 * @retval -ERRINVAL Invalid parameters
 */
int32_t Kfifo_Init(Kfifo_t *fifo, void *buffer, uint32_t size, size_t esize)
{
    if (fifo == NULL || buffer == NULL || size == 0 || esize == 0) {
        return -ERR_INVAL;
    }
    size /= esize;
    if (size < 2 || !is_power_of_2(size)) {
        return -ERR_INVAL;
    }
    fifo->in = 0;
    fifo->out = 0;
    fifo->esize = esize;
    fifo->data = buffer;
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
uint32_t Kfifo_In(Kfifo_t *fifo, const void *buf, uint32_t len)
{
	uint32_t unused_len = 0;
    
	unused_len = Kfifo_GetUnused(fifo);
	if (len > unused_len) {
        len = unused_len;
    }

	Kfifo_CopyIn(fifo, buf, len, fifo->in);
	fifo->in += len;
	return len;
}

/**
 * @brief Get data from the FIFO
 * @param fifo Pointer to the FIFO structure
 * @param buf  Pointer to the storage buffer (elements)
 * @param len  Number of elements to get
 * @return Number of elements actually copied
 */
uint32_t Kfifo_Out(Kfifo_t *fifo, void *buf, uint32_t len)
{
	len = Kfifo_OutPeek(fifo, buf, len);
	fifo->out += len;
	return len;
}

/**
 * @brief Peek data from the FIFO without removing it
 * @param fifo Pointer to the FIFO structure
 * @param buf  Pointer to the storage buffer (elements)
 * @param len  Number of elements to peek
 * @return Number of elements actually copied
 */
uint32_t Kfifo_OutPeek(Kfifo_t *fifo, void *buf, uint32_t len)
{
	uint32_t l;
    
	l = fifo->in - fifo->out;
	if (len > l)
		len = l;

	Kfifo_CopyOut(fifo, buf, len, fifo->out);
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
unsigned int Kfifo_OutLinear(Kfifo_t *fifo, uint32_t *tail, uint32_t n)
{
	uint32_t size = fifo->mask + 1;
	uint32_t off = fifo->out & fifo->mask;

	if (tail)
		*tail = off;

	return MIN_U32(MIN_U32(n, fifo->in - fifo->out), size - off);
}

void Kfifo_SkipCount(Kfifo_t *fifo, uint32_t cnt)
{
    uint32_t used = fifo->in - fifo->out;
    if (cnt > used) { 
        cnt = used; 
    }
    
    fifo->out += cnt;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Get the unused space in the FIFO
 * @param fifo Pointer to the FIFO structure
 * @return Number of unused elements in the FIFO
 */
static inline uint32_t Kfifo_GetUnused(const Kfifo_t *fifo)
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
static void Kfifo_CopyIn(Kfifo_t *fifo, const void *src, uint32_t len, uint32_t off)
{
	uint32_t size = fifo->mask + 1;
	uint32_t esize = fifo->esize;
	uint32_t l;

	off &= fifo->mask;
	if (esize != 1) {
		off *= esize;
		size *= esize;
		len *= esize;
	}
	l = MIN_U32(len, size - off);

	memcpy(fifo->data + off, src, l);
	memcpy(fifo->data, (const uint8_t*)src + l, len - l);

    /* make sure that the data in the fifo is up to date before
     * incrementing the fifo->in index counter */
    KFIFO_WMB();
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
static void Kfifo_CopyOut(Kfifo_t *fifo, void *dst, uint32_t len, uint32_t off)
{
	uint32_t size = fifo->mask + 1;
	uint32_t esize = fifo->esize;
	uint32_t l;

	off &= fifo->mask;
	if (esize != 1) {
		off *= esize;
		size *= esize;
		len *= esize;
	}
	l = MIN_U32(len, size - off);

	memcpy((uint8_t*)dst, fifo->data + off, l);
	memcpy((uint8_t*)dst + l, fifo->data, len - l);

    /* make sure that the data is copied before
     * incrementing the fifo->out index counter */
    KFIFO_WMB();
}
