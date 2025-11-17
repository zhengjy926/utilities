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

/* Private functions ---------------------------------------------------------*/
/**
 * @brief Get the unused space in the FIFO
 * @param fifo Pointer to the FIFO structure
 * @return Number of unused elements in the FIFO
 */
static inline unsigned int kfifo_unused(const kfifo_t *fifo)
{
    return kfifo_size(fifo) - kfifo_len(fifo);
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
    unsigned int size_elems = fifo->mask + 1U;
    unsigned int esize      = fifo->esize;
    unsigned int off_bytes;
    unsigned int size_bytes;
    unsigned int len_bytes;
    unsigned int l_bytes;
    uint8_t     *dst_bytes;
    const uint8_t *src_bytes;

    off       &= fifo->mask;
    dst_bytes  = (uint8_t *)fifo->data;
    src_bytes  = (const uint8_t *)src;

    if (esize != 1U) {
        off_bytes  = off * esize;
        size_bytes = size_elems * esize;
        len_bytes  = len * esize;
    } else {
        off_bytes  = off;
        size_bytes = size_elems;
        len_bytes  = len;
    }

    l_bytes = min(len_bytes, size_bytes - off_bytes);

    (void)memcpy(&dst_bytes[off_bytes], src_bytes, (size_t)l_bytes);
    (void)memcpy(dst_bytes, &src_bytes[l_bytes], (size_t)(len_bytes - l_bytes));

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
    unsigned int size_elems = fifo->mask + 1U;
    unsigned int esize      = fifo->esize;
    unsigned int off_bytes;
    unsigned int size_bytes;
    unsigned int len_bytes;
    unsigned int l_bytes;
    uint8_t     *dst_bytes;
    const uint8_t *src_bytes;

    off       &= fifo->mask;
    dst_bytes  = (uint8_t *)dst;
    src_bytes  = (const uint8_t *)fifo->data;

    if (esize != 1U) {
        off_bytes  = off * esize;
        size_bytes = size_elems * esize;
        len_bytes  = len * esize;
    } else {
        off_bytes  = off;
        size_bytes = size_elems;
        len_bytes  = len;
    }

    l_bytes = min(len_bytes, size_bytes - off_bytes);

    (void)memcpy(dst_bytes, &src_bytes[off_bytes], (size_t)l_bytes);
    (void)memcpy(&dst_bytes[l_bytes], src_bytes, (size_t)(len_bytes - l_bytes));

    /* make sure that the data is copied before
     * incrementing the fifo->out index counter */
    __DMB();
}

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
    unsigned int elems;

    if ((fifo == NULL) || (buffer == NULL) || (size == 0U) || (esize == 0U)) {
        return -EINVAL;
    }

    /* convert total bytes -> number of elements */
    elems = size / esize;
    if (elems < 2U) {
        return -EINVAL;
    }

    /* ensure element count is a power of 2 for mask-based index */
    if (is_power_of_2(elems) == 0U) {
        elems = rounddown_pow_of_two(elems);
        if (elems < 2U) {
            return -EINVAL;
        }
    }

    fifo->in    = 0U;
    fifo->out   = 0U;
    fifo->esize = esize;
    fifo->data  = buffer;
    fifo->mask  = elems - 1U;

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
    unsigned int unused_len;

    if ((fifo == NULL) || (buf == NULL) || (len == 0U)) {
        return 0U;
    }

    unused_len = kfifo_unused(fifo);
    if (len > unused_len) {
        len = unused_len;
    }

    if (len == 0U) {
        return 0U;
    }

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
    unsigned int avail;

    if ((fifo == NULL) || (buf == NULL) || (len == 0U)) {
        return 0U;
    }

    avail = fifo->in - fifo->out;
    if (len > avail) {
        len = avail;
    }

    if (len == 0U) {
        return 0U;
    }

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
    unsigned int size;
    unsigned int off;
    unsigned int avail;

    if ((fifo == NULL) || (n == 0U)) {
        if (tail != NULL) {
            *tail = 0U;
        }
        return 0U;
    }

    size  = fifo->mask + 1U;
    off   = fifo->out & fifo->mask;
    avail = fifo->in - fifo->out;

    if (tail != NULL) {
        *tail = off;
    }

    return min3(n, avail, size - off);
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
    if ((fifo == NULL) || (buf == NULL) || (len == 0U)) {
        return 0U;
    }

    len = kfifo_out_peek(fifo, buf, len);
    if (len == 0U) {
        return 0U;
    }

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

/**************************** End of file *************************************/
