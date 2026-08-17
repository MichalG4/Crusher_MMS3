#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

/* Result / exception codes returned by modbus_read_register() and
 * modbus_write_register().
 *
 *   0x00               -> MODBUS_NO_EXCEPTION, request succeeded
 *   0x01 - 0x7F         -> exception code reported by the slave device
 *                          (see the specific device's manual)
 *   MODBUS_INVALID_CRC  -> response received but CRC check failed
 *   MODBUS_TIMEOUT      -> no (complete) response received in time
 */
typedef uint8_t exception_code_t;

typedef enum {
    MODBUS_FN_READ_HOLD_REGISTERS      = 0x03,
    MODBUS_FN_READ_INPUT_REGISTERS     = 0x04,
    MODBUS_FN_WRITE_SINGLE_REGISTER    = 0x06,
    MODBUS_FN_WRITE_MULTIPLE_REGISTERS = 0x10
} modbus_function_t;

enum {
    MODBUS_NO_EXCEPTION = 0x00,
    MODBUS_INVALID_CRC  = 0xFE,
    MODBUS_TIMEOUT      = 0xFF
};

/* Represents a single Modbus RTU slave device reachable over a UART/RS485
 * link. Several devices can share the same huart (i.e. the same RS485 bus)
 * as long as communication does not happen concurrently. */
typedef struct {
    UART_HandleTypeDef *huart;
    uint8_t              slave_address;

    /* Optional RS485 transceiver driver-enable (DE, often tied to ~RE) pin.
     * Set de_port = NULL if your transceiver directions itself automatically
     * and you don't need to drive this manually. */
    GPIO_TypeDef         *de_port;
    uint16_t              de_pin;

    uint32_t              timeout_ms; /* HAL_UART timeout for responses, ms */
} ModbusRTUDevice;

uint16_t modbus_crc16(const uint8_t *data, uint16_t length);

/* Assumes that data ends with the CRC16 split into low and high byte. */
bool modbus_verify_crc16(const uint8_t *data, uint16_t length);

/* Basic init, no DE pin (transceiver directions itself automatically). */
void modbus_device_init(ModbusRTUDevice *dev,
                         UART_HandleTypeDef *huart,
                         uint8_t slave_address);

/* Init with a manually-driven RS485 DE/~RE pin (most MAX485-style boards). */
void modbus_device_init_rs485(ModbusRTUDevice *dev,
                               UART_HandleTypeDef *huart,
                               uint8_t slave_address,
                               GPIO_TypeDef *de_port,
                               uint16_t de_pin);

exception_code_t modbus_read_register(ModbusRTUDevice *dev, uint16_t address,
                                       uint16_t *value, bool read_only);

exception_code_t modbus_write_register(ModbusRTUDevice *dev, uint16_t address,
                                        uint16_t value);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_RTU_H */