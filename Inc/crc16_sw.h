#ifndef CRC_H
#define CRC_H

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

uint16_t CRC16_8005(uint16_t init_value, uint16_t xor_out_value, 
                    const uint8_t *buffer, size_t len);
uint16_t CRC16_1021(uint16_t init_value, uint16_t xor_out_value, bool reversed, 
                    const uint8_t *buffer, size_t len);

uint16_t CRC16_8005_Update(uint16_t crc, const uint8_t *buffer, size_t len);
uint16_t CRC16_1021_Update(uint16_t crc, bool reversed, 
                           const uint8_t *buffer, size_t len);

/**
 * @defgroup CRC16-IBM
 * @{
 */

static inline uint16_t CRC16_IBM(const uint8_t *buffer, size_t len)
{
    return CRC16_8005(0x0000, 0x0000, buffer, len);
}

static inline uint16_t CRC16_IBM_Init(void)
{
    return 0x0000U;
}

static inline uint16_t CRC16_IBM_Update(uint16_t crc, const uint8_t *buffer, size_t len)
{
    return CRC16_8005_Update(crc, buffer, len);
}

static inline uint16_t CRC16_IBM_Finalize(uint16_t crc)
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
static inline uint16_t CRC16_MAXIM(const uint8_t *buffer, size_t len)
{
    return CRC16_8005(0x0000, 0xFFFF, buffer, len);
}

static inline uint16_t CRC16_MAXIM_Init(void)
{
    return 0x0000U;
}

static inline uint16_t CRC16_MAXIM_Update(uint16_t crc, const uint8_t *buffer, size_t len)
{
    return CRC16_8005_Update(crc, buffer, len);
}

static inline uint16_t CRC16_MAXIM_Finalize(uint16_t crc)
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
static inline uint16_t CRC16_USB(const uint8_t *buffer, size_t len)
{
    return CRC16_8005(0xFFFF, 0xFFFF, buffer, len);
}

static inline uint16_t CRC16_USB_Init(void)
{
    return 0xFFFFU;
}

static inline uint16_t CRC16_USB_Update(uint16_t crc, const uint8_t *buffer, size_t len)
{
    return CRC16_8005_Update(crc, buffer, len);
}

static inline uint16_t CRC16_USB_Finalize(uint16_t crc)
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
static inline uint16_t CRC16_MODBUS(const uint8_t *buffer, size_t len)
{
    return CRC16_8005(0xFFFF, 0x0000, buffer, len);
}

static inline uint16_t CRC16_MODBUS_Init(void)
{
    return 0xFFFFU;
}

static inline uint16_t CRC16_MODBUS_Update(uint16_t crc, const uint8_t *buffer, size_t len)
{
    return CRC16_8005_Update(crc, buffer, len);
}

static inline uint16_t CRC16_MODBUS_Finalize(uint16_t crc)
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
static inline uint16_t CRC16_CCITT(const uint8_t *buffer, size_t len)
{
    return CRC16_1021(0x0000, 0x0000, true, buffer, len);
}

static inline uint16_t CRC16_CCITT_Init(void)
{
    return 0x0000U;
}

static inline uint16_t CRC16_CCITT_Update(uint16_t crc, const uint8_t *buffer, size_t len)
{
    return CRC16_1021_Update(crc, true, buffer, len);
}

static inline uint16_t CRC16_CCITT_Finalize(uint16_t crc)
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
static inline uint16_t CRC16_CCITT_FALSE(const uint8_t *buffer, size_t len)
{
    return CRC16_1021(0xFFFF, 0x0000, false, buffer, len);
}

static inline uint16_t CRC16_CCITT_FALSE_Init(void)
{
    return 0xFFFFU;
}

static inline uint16_t CRC16_CCITT_FALSE_Update(uint16_t crc, const uint8_t *buffer, size_t len)
{
    return CRC16_1021_Update(crc, false, buffer, len);
}

static inline uint16_t CRC16_CCITT_FALSE_Finalize(uint16_t crc)
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
static inline uint16_t CRC16_X25(const uint8_t *buffer, size_t len)
{
    return CRC16_1021(0xFFFF, 0xFFFF, true, buffer, len);
}

static inline uint16_t CRC16_X25_Init(void)
{
    return 0xFFFFU;
}

static inline uint16_t CRC16_X25_Update(uint16_t crc, const uint8_t *buffer, size_t len)
{
    return CRC16_1021_Update(crc, true, buffer, len);
}

static inline uint16_t CRC16_X25_Finalize(uint16_t crc)
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
static inline uint16_t CRC16_XMODEM(const uint8_t *buffer, size_t len)
{
    return CRC16_1021(0x0000, 0x0000, false, buffer, len);
}

static inline uint16_t CRC16_XMODEM_Init(void)
{
    return 0x0000U;
}

static inline uint16_t CRC16_XMODEM_Update(uint16_t crc, const uint8_t *buffer, size_t len)
{
    return CRC16_1021_Update(crc, false, buffer, len);
}

static inline uint16_t CRC16_XMODEM_Finalize(uint16_t crc)
{
    return (uint16_t)(crc ^ 0x0000U);
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CRC_H */





