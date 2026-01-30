#ifndef __CRC_H
#define __CRC_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
/* Exported define -----------------------------------------------------------*/

/* Exported typedef ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variable prototypes ----------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

uint16_t crc16_8005(uint16_t init_value, uint16_t xor_out_value, 
                    const uint8_t *buffer, size_t len);
uint16_t crc16_1021(uint16_t init_value, uint16_t xor_out_value, bool reversed, 
                    const uint8_t *buffer, size_t len);

uint16_t crc16_8005_update(uint16_t crc, const uint8_t *buffer, size_t len);
uint16_t crc16_1021_update(uint16_t crc, bool reversed, 
                           const uint8_t *buffer, size_t len);

/**
 * @defgroup CRC16-IBM
 * @{
 */

static inline uint16_t crc16_ibm(const uint8_t *buffer, size_t len)
{
    return crc16_8005(0x0000, 0x0000, buffer, len);
}

static inline uint16_t crc16_ibm_init(void)
{
    return 0x0000U;
}

static inline uint16_t crc16_ibm_update(uint16_t crc, const uint8_t *buffer, size_t len)
{
    return crc16_8005_update(crc, buffer, len);
}

static inline uint16_t crc16_ibm_finalize(uint16_t crc)
{
    return (uint16_t)(crc ^ 0x0000U);
}

/**
 * @}
 */

/**
 * @defgroup CRC16-MAXIM
 * @{
 */
static inline uint16_t crc16_maxim(const uint8_t *buffer, size_t len)
{
    return crc16_8005(0x0000, 0xFFFF, buffer, len);
}

static inline uint16_t crc16_maxim_init(void)
{
    return 0x0000U;
}

static inline uint16_t crc16_maxim_update(uint16_t crc, const uint8_t *buffer, size_t len)
{
    return crc16_8005_update(crc, buffer, len);
}

static inline uint16_t crc16_maxim_finalize(uint16_t crc)
{
    return (uint16_t)(crc ^ 0xFFFFU);
}

/**
 * @}
 */

/**
 * @defgroup CRC16-USB
 * @{
 */
static inline uint16_t crc16_usb(const uint8_t *buffer, size_t len)
{
    return crc16_8005(0xFFFF, 0xFFFF, buffer, len);
}

static inline uint16_t crc16_usb_init(void)
{
    return 0xFFFFU;
}

static inline uint16_t crc16_usb_update(uint16_t crc, const uint8_t *buffer, size_t len)
{
    return crc16_8005_update(crc, buffer, len);
}

static inline uint16_t crc16_usb_finalize(uint16_t crc)
{
    return (uint16_t)(crc ^ 0xFFFFU);
}

/**
 * @}
 */

/**
 * @defgroup CRC16-MODBUS
 * @{
 */
static inline uint16_t crc16_modbus(const uint8_t *buffer, size_t len)
{
    return crc16_8005(0xFFFF, 0x0000, buffer, len);
}

static inline uint16_t crc16_modbus_init(void)
{
    return 0xFFFFU;
}

static inline uint16_t crc16_modbus_update(uint16_t crc, const uint8_t *buffer, size_t len)
{
    return crc16_8005_update(crc, buffer, len);
}

static inline uint16_t crc16_modbus_finalize(uint16_t crc)
{
    return (uint16_t)(crc ^ 0x0000U);
}

/**
 * @}
 */

/**
 * @defgroup CRC16-CCITT
 * @{
 */
static inline uint16_t crc16_ccitt(const uint8_t *buffer, size_t len)
{
    return crc16_1021(0x0000, 0x0000, true, buffer, len);
}

static inline uint16_t crc16_ccitt_init(void)
{
    return 0x0000U;
}

static inline uint16_t crc16_ccitt_update(uint16_t crc, const uint8_t *buffer, size_t len)
{
    return crc16_1021_update(crc, true, buffer, len);
}

static inline uint16_t crc16_ccitt_finalize(uint16_t crc)
{
    return (uint16_t)(crc ^ 0x0000U);
}

/**
 * @}
 */

/**
 * @defgroup CRC16-CCITT-FALSE
 * @{
 */
static inline uint16_t crc16_ccitt_false(const uint8_t *buffer, size_t len)
{
    return crc16_1021(0xFFFF, 0x0000, false, buffer, len);
}

static inline uint16_t crc16_ccitt_false_init(void)
{
    return 0xFFFFU;
}

static inline uint16_t crc16_ccitt_false_update(uint16_t crc, const uint8_t *buffer, size_t len)
{
    return crc16_1021_update(crc, false, buffer, len);
}

static inline uint16_t crc16_ccitt_false_finalize(uint16_t crc)
{
    return (uint16_t)(crc ^ 0x0000U);
}

/**
 * @}
 */

/**
 * @defgroup CRC16-X25
 * @{
 */
static inline uint16_t crc16_x25(const uint8_t *buffer, size_t len)
{
    return crc16_1021(0xFFFF, 0xFFFF, true, buffer, len);
}

static inline uint16_t crc16_x25_init(void)
{
    return 0xFFFFU;
}

static inline uint16_t crc16_x25_update(uint16_t crc, const uint8_t *buffer, size_t len)
{
    return crc16_1021_update(crc, true, buffer, len);
}

static inline uint16_t crc16_x25_finalize(uint16_t crc)
{
    return (uint16_t)(crc ^ 0xFFFFU);
}

/**
 * @}
 */

/**
 * @defgroup CRC16-XMODEM
 * @{
 */
static inline uint16_t crc16_xmodem(const uint8_t *buffer, size_t len)
{
    return crc16_1021(0x0000, 0x0000, false, buffer, len);
}

static inline uint16_t crc16_xmodem_init(void)
{
    return 0x0000U;
}

static inline uint16_t crc16_xmodem_update(uint16_t crc, const uint8_t *buffer, size_t len)
{
    return crc16_1021_update(crc, false, buffer, len);
}

static inline uint16_t crc16_xmodem_finalize(uint16_t crc)
{
    return (uint16_t)(crc ^ 0x0000U);
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CRC_H */





