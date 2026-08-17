#pragma once
#include "../../include/chainbus_header_common.h"
#include <stdint.h>
#include <stddef.h>

void chainbus_hat_protocol_init(Hat_position position);

void chainbus_hat_protocol_config_write(Hat_position position, int config_pin, int state);

#define CHAINBUS_PIN_UART_1_OE 0		// Low (L)  - Disconnected (Output Enable)
#define CHAINBUS_PIN_UART_1_RE_N 1		// High (H) - Disabled (RS485 Receiver Enable)
#define CHAINBUS_PIN_UART_1_SHDN_N 2	// High (H) - Disabled (RS485 Shutdown)
#define CHAINBUS_PIN_UART_1_BONUS_O1 3	// Low (L)  - Bonus Out J12
#define CHAINBUS_PIN_UART_1_BONUS_O2 4	// Low (L)  - Bonus Out J12
#define CHAINBUS_PIN_STATUS_LED 5		// General Purpose LED
#define CHAINBUS_PIN_UART_2_OE 6		// Low (L)  - Disconnected (Output Enable)
#define CHAINBUS_PIN_UART_2_RE_N 7		// High (H) - Disabled (RS485 Receiver Enable)
#define CHAINBUS_PIN_UART_2_SHDN_N 8	// High (H) - Disabled (RS485 Shutdown)
#define CHAINBUS_PIN_UART_2_BONUS_O1 9	// Low (L)  - Bonus Out J13
#define CHAINBUS_PIN_UART_2_BONUS_O2 10 // Low (L)  - Bonus Out J13
#define CHAINBUS_PIN_SPI_1_ENABLE 11	// Low (L)  - Disconnected (SPI Buffer OE)
#define CHAINBUS_PIN_SPI_1_BONUS_I1 12	// Bonus In J9
#define CHAINBUS_PIN_SPI_1_BONUS_I2 13	// Bonus In J9
#define CHAINBUS_PIN_SPI_1_BONUS_I3 14	// Bonus In J9
#define CHAINBUS_PIN_SPI_1_BONUS_O1 15	// Bonus Out J9

void chainbus_hat_protocol_i2c_write(Hat_position position, uint8_t addr, const uint8_t *data, size_t len);
void chainbus_hat_protocol_i2c_read(Hat_position position, uint8_t addr, uint8_t *data, size_t len);
void chainbus_hat_protocol_I2C_write_read(Hat_position position, uint8_t addr, uint8_t *write_data, int write_len, uint8_t *read_data, int read_len);
