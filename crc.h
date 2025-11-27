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

uint16_t crc16_8005(uint16_t      init_value, uint16_t xor_out_value, 
                    const uint8_t *buffer,    size_t   len);

uint16_t crc16_1021(uint16_t init_value, uint16_t xor_out_value, bool reversed, 
                    const uint8_t *buffer, size_t len);

static inline uint16_t crc16_ibm(const uint8_t *buffer, size_t len)
{
    return crc16_8005(0x0000, 0x0000, buffer, len);
}

static inline uint16_t crc16_maxim(const uint8_t *buffer, size_t len)
{
    return crc16_8005(0x0000, 0xFFFF, buffer, len);
}

static inline uint16_t crc16_usb(const uint8_t *buffer, size_t len)
{
    return crc16_8005(0xFFFF, 0xFFFF, buffer, len);
}

static inline uint16_t crc16_modbus(const uint8_t *buffer, size_t len)
{
    return crc16_8005(0xFFFF, 0x0000, buffer, len);
}

static inline uint16_t crc16_ccitt(const uint8_t *buffer, size_t len)
{
    return crc16_1021(0x0000, 0x0000, true, buffer, len);
}

static inline uint16_t crc16_ccitt_false(const uint8_t *buffer, size_t len)
{
    return crc16_1021(0xFFFF, 0x0000, false, buffer, len);
}

static inline uint16_t crc16_x25(const uint8_t *buffer, size_t len)
{
    return crc16_1021(0xFFFF, 0xFFFF, true, buffer, len);
}

static inline uint16_t crc16_xmodem(const uint8_t *buffer, size_t len)
{
    return crc16_1021(0x0000, 0x0000, false, buffer, len);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CRC_H */





