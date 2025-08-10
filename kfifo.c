/**
 *******************************************************************************
 * @file        : kfifo.c
 * @author      : ZJY
 * @version     : V1.0
 * @date        : 2024-07-16
 * @brief       : Kernel FIFO implementation
 * @attention   : xxx
 *******************************************************************************
 * @history     :
 * | Version | Description |
 * |---------|-------------|
 * | V1.0    | Initial implementation |
 *******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Exported variables  -------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "kfifo.h"
#include "math/log2.h"
#include "math/minmax.h"
#include <string.h>

/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static inline size_t kfifo_unused(kfifo_t *fifo);
static void kfifo_copy_in(kfifo_t *fifo, const void *src, size_t len, size_t off);
static void kfifo_copy_out(kfifo_t *fifo, void *dst, size_t len, size_t off);

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize a FIFO buffer
 * @param fifo   Pointer to the FIFO structure to initialize
 * @param buffer Pointer to the storage buffer
 * @param size   Size of the storage buffer in bytes (Note: size must be a power of 2)
 * @param esize  Size of each element in bytes
 * @retval 0       Success
 * @retval -EINVAL Invalid parameters (esize is 0 or size is too small)
 */
int kfifo_init(kfifo_t *fifo, void *buffer, size_t size, size_t esize)
{  
    if (!fifo || !buffer || size == 0 || esize == 0) {
        return -EINVAL;
    }

    /* convert total bytes -> number of elements */
    size_t elems = size / esize;
    if (elems < 2) {
        return -EINVAL;
    }

    if (!is_power_of_2((unsigned int)elems)) {
        elems = rounddown_pow_of_two((unsigned int)elems);
        if (elems < 2)
            return -EINVAL;
    }

    fifo->in = 0;
    fifo->out = 0;
    fifo->esize = esize;
    fifo->data = buffer;
    fifo->mask = elems - 1;

    return 0;
}

/**
 * @brief Put data into the FIFO
 * @param fifo Pointer to the FIFO structure
 * @param buf  Pointer to the data to be added
 * @param len  Number of elements to be added
 * @return Number of elements actually copied
 */
size_t kfifo_in(kfifo_t *fifo, const void *buf, size_t len)
{
    if (!fifo || !buf || len == 0) {
        return 0;
    }
    
	size_t unused = kfifo_unused(fifo);
    
	unused = kfifo_unused(fifo);
	if (len > unused)
		len = unused;
    
    if (len == 0) {
        return 0;
    }
    
	kfifo_copy_in(fifo, buf, len, fifo->in);
    fifo->in += len;
    
	return len;
}

/**
 * @brief Put data into the FIFO with interrupt protection
 * @param fifo Pointer to the FIFO structure
 * @param buf  Pointer to the data to be added
 * @param len  Number of elements to be added
 * @return Number of elements actually copied
 */
size_t kfifo_in_locked (kfifo_t *fifo, const void *buf, size_t len)
{
    size_t ret;
    uint32_t primask = __get_PRIMASK(); 
    __disable_irq();
    ret = kfifo_in(fifo, buf, len);
    __set_PRIMASK(primask);
    return ret;
}

/**
 * @brief Peek data from the FIFO without removing it
 * @param fifo Pointer to the FIFO structure
 * @param buf  Pointer to the storage buffer
 * @param len  Number of elements to peek
 * @return Number of elements actually copied
 */
size_t kfifo_out_peek(kfifo_t *fifo, void *buf, size_t len)
{
    if (!fifo || !buf || len == 0) {
        return 0;
    }
    
    size_t used = fifo->in - fifo->out;
    if (len > used) {
        len = used;
    } else if(len == 0) {
        return 0;
    }

	kfifo_copy_out(fifo, buf, len, fifo->out);
	return len;
}

/**
 * @brief Get data from the FIFO
 * @param fifo Pointer to the FIFO structure
 * @param buf  Pointer to the storage buffer
 * @param len  Number of elements to get
 * @return Number of elements actually copied
 */
size_t kfifo_out(kfifo_t *fifo, void *buf, size_t len)
{
    if (!fifo || !buf || len == 0) {
        return 0;
    }
    
	len = kfifo_out_peek(fifo, buf, len);
    if (len == 0) {
        return 0;
    }
    
    fifo->out += len;
	return len;
}

/**
 * @brief Get data from the FIFO with interrupt protection
 * @param fifo Pointer to the FIFO structure
 * @param buf  Pointer to the storage buffer
 * @param len  Number of elements to get
 * @return Number of elements actually copied
 */
size_t kfifo_out_locked(kfifo_t *fifo, void *buf, size_t len)
{
    size_t ret;
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
	ret = kfifo_out(fifo, buf, len);
    __set_PRIMASK(primask);
	return ret;
}

/**
 * @brief Get the length of the linear space that can be read
 * @param fifo Pointer to the FIFO structure
 * @param tail Pointer to return the starting position that can be read in the linear space
 * @param n    Number of elements requested to be read
 * @return Available count till the end of data or till the end of the buffer
 * @note With only one concurrent reader and one concurrent writer,
 *       you don't need extra locking to use this function.
 */
size_t kfifo_out_linear(kfifo_t *fifo, size_t *tail, size_t n)
{
    if (!fifo) {
        return 0;
    }
    
	size_t size = fifo->mask + 1;
	size_t off = fifo->out & fifo->mask;

	if (tail)
		*tail = off;
    
    return min3(n, fifo->in - fifo->out, size - off);
}

/**
 * @brief Get the length of the linear space that can be read with interrupt protection
 * @param fifo Pointer to the FIFO structure
 * @param tail Pointer to return the starting position that can be read in the linear space
 * @param n    Number of elements requested to be read
 * @return Available count till the end of data or till the end of the buffer
 */
size_t kfifo_out_linear_locked(kfifo_t *fifo, size_t *tail, size_t n)
{
    size_t ret;
    uint32_t primask = __get_PRIMASK();
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
static inline size_t kfifo_unused(kfifo_t *fifo)
{
    return (fifo->mask + 1) - (fifo->in - fifo->out);
}

/**
 * @brief Copy data into the FIFO buffer
 * @param fifo Pointer to the FIFO structure
 * @param src  Pointer to the source buffer
 * @param len  Number of elements to be copied
 * @param off  Offset in the destination buffer
 * @details This function handles circular buffer wraparound and ensures
 *          data consistency with memory barriers.
 */
static void kfifo_copy_in(kfifo_t *fifo, const void *src, size_t len, size_t off)
{
	size_t size = fifo->mask + 1;
	size_t esize = fifo->esize;
	size_t l;

	off &= fifo->mask;
	if (esize != 1) {
		off *= esize;
		size *= esize;
		len *= esize;
	}
	l = min(len, size - off);

	memcpy((uint8_t*)fifo->data + off, (uint8_t*)src, l);
	memcpy((uint8_t*)fifo->data, (uint8_t*)src + l, len - l);
	/*
	 * make sure that the data in the fifo is up to date before
	 * incrementing the fifo->in index counter
	 */
	__DMB();
}

/**
 * @brief Copy data from the FIFO buffer
 * @param fifo Pointer to the FIFO structure
 * @param dst  Pointer to the destination buffer
 * @param len  Number of elements to be copied
 * @param off  Offset in the source buffer
 * @details This function handles circular buffer wraparound and ensures
 *          data consistency with memory barriers.
 */
static void kfifo_copy_out(kfifo_t *fifo, void *dst, size_t len, size_t off)
{
	size_t size = fifo->mask + 1;
	size_t esize = fifo->esize;
	size_t l;

	off &= fifo->mask;
	if (esize != 1) {
		off *= esize;
		size *= esize;
		len *= esize;
	}
	l = min(len, size - off);

	memcpy((uint8_t*)dst, (uint8_t*)fifo->data + off, l);
	memcpy((uint8_t*)dst + l, (uint8_t*)fifo->data, len - l);
	/*
	 * make sure that the data is copied before
	 * incrementing the fifo->out index counter
	 */
	__DMB();
}
/**************************** End of file *************************************/




