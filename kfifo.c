/**
  ******************************************************************************
  * @file    : kfifo.c
  * @brief   : kfifo
  * @author  : zhengjy
  * @version : v1.0 
  ******************************************************************************
  * @history : 
  *
  ******************************************************************************
  */
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
static void kfifo_copy_in(kfifo_t *fifo, const void  *src, 
                          size_t len,  size_t    off);
static void kfifo_copy_out(kfifo_t *fifo, void     *dst,
		                   size_t len,  size_t off);
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
  * @brief  initialize the fifo
  * @param  fifo: address of the fifo to be used
  * @param  buffer: pointer to the storage buffer
  * @param  size: size of the storage buffer(in bytes). Note: size must be a power of 2
  * @retval 0: success
  */
int kfifo_init(kfifo_t *fifo, void *buffer, size_t size, size_t esize)
{  
    /* Make sure that (size * esize) does not exceed the platform address space */
    if (esize == 0)
        return -EINVAL;

	size /= esize;

	if (!is_power_of_2(size))
        size = rounddown_pow_of_two(size);
    
	fifo->in    = 0;
	fifo->out   = 0;
	fifo->esize = esize;
	fifo->data  = buffer;

	if (size < 2) {
		fifo->mask = 0;
		return -EINVAL;
	}
	fifo->mask = size - 1;

	return 0;
}

/**
  * @brief  put data into the fifo
  * @param  fifo: address of the fifo to be used
  * @param  buf : the data to be added
  * @param  len : number of elements to be added
  * @retval the number of copied elements.
  */
size_t kfifo_in(kfifo_t *fifo, const void *buf, size_t len)
{
	size_t l;
    
	l = kfifo_unused(fifo);
	if (len > l)
		len = l;

	kfifo_copy_in(fifo, buf, len, fifo->in);
    fifo->in += len;
    
	return len;
}

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
  * @brief  peek data from the fifo
  * @param  fifo: address of the fifo to be used
  * @param  buf: pointer to the storage buffer
  * @param  len: number of elements to peek
  * @retval returns the numbers of elements copied.
  */
size_t kfifo_out_peek(kfifo_t *fifo, void *buf, size_t len)
{
	size_t l;

	l = fifo->in - fifo->out;
	if (len > l)
		len = l;

	kfifo_copy_out(fifo, buf, len, fifo->out);
	return len;
}

/**
  * @brief  get data from the fifo
  * @param  fifo: address of the fifo to be used
  * @param  buf: pointer to the storage buffer
  * @param  len: number of elements to get
  * @retval returns the numbers of elements copied.
  */
size_t kfifo_out(kfifo_t *fifo, void *buf, size_t len)
{  
	len = kfifo_out_peek(fifo, buf, len);
    fifo->out += len;
    
	return len;
}

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
 * @brief  Get the length of the linear space that can be read
 * @param  fifo: Address of the fifo to be used.
 * @param  tail: Return the starting position that can be read in the linear space.
 * @param  n   : The number of data requested to be read.
 * @retval It returns the available count till the end of data 
 *         or till the end of the buffer.
 * @note   That with only one concurrent reader and one concurrent writer,
 *         you don't need extra locking to use this function.
 */
size_t kfifo_out_linear(kfifo_t *fifo, size_t *tail, size_t n)
{
	size_t size = fifo->mask + 1;
	size_t off = fifo->out & fifo->mask;

	if (tail)
		*tail = off;
    
    return min3(n, fifo->in - fifo->out, size - off);
}

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
  * @brief  get the unused space in the fifo
  * @param  fifo: address of the fifo to be used
  * @retval the unused space in the fifo
  */
static inline size_t kfifo_unused(kfifo_t *fifo)
{
    return (fifo->mask + 1) - (fifo->in - fifo->out);
}

/**
  * @brief  copy data to the fifo
  * @param  fifo: address of the fifo to be used
  * @param  src: pointer to the source buffer
  * @param  len: number of elements to be copied
  * @param  off: offset to the destination buffer
  * @retval none
  */
static void kfifo_copy_in(kfifo_t     *fifo, const void  *src, 
                          size_t len,  size_t off)
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
  * @brief  copy data from the fifo
  * @param  fifo: address of the fifo to be used
  * @param  dst: pointer to the destination buffer
  * @param  len: number of elements to be copied
  * @param  off: offset to the source buffer
  * @retval none
  */
static void kfifo_copy_out(kfifo_t     *fifo, void        *dst,
		                   size_t len,  size_t off)
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




