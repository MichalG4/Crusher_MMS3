#include "chainbus_hat_protocol.h"
#include "../../include/chainbus_header_hat.h"
#include "../../include/chainbus_header_common.h"

void chainbus_hat_protocol_init(Hat_position position)
{
	// Select the target hat position
	chainbus_select_hat(position);
	chainbus_I2C_config_speed(chainbus_I2C_config_speed_standard);

	// Default Output values:
	// Port 0:
	// P00 (UART 1 OE)        = 0 (Low)
	// P01 (UART 1 RE_N)      = 1 (High)
	// P02 (UART 1 SHDN_N)    = 1 (High)
	// P03 (UART 1 BONUS OUT) = 0 (Low)
	// P04 (UART 1 BONUS OUT) = 0 (Low)
	// P05 (STATUS LED)       = 0 (Low)
	// P06 (UART 2 OE)        = 0 (Low)
	// P07 (UART 2 RE_N)      = 1 (High)
	// Binary: 10000110b -> 0x86
	//
	// Port 1:
	// P10 (UART 2 SHDN_N)    = 1 (High)
	// P11 (UART 2 BONUS OUT) = 0 (Low)
	// P12 (UART 2 BONUS OUT) = 0 (Low)
	// P13 (SPI 1 ENABLE)     = 0 (Low)
	// P14 (SPI 1 BONUS IN)   = 0 (Input)
	// P15 (SPI 1 BONUS IN)   = 0 (Input)
	// P16 (SPI 1 BONUS IN)   = 0 (Input)
	// P17 (SPI 1 BONUS OUT)  = 0 (Low)
	// Binary: 00000001b -> 0x01
	uint8_t out_data[3] = {0x02, 0x86, 0x01};
	chainbus_I2C_write(0x20, out_data, 3);

	// Pin Directions (0 = Output, 1 = Input):
	// Port 0: All outputs -> 0x00
	// Port 1: P14, P15, P16 are inputs (1), others are outputs (0) -> 0x70
	uint8_t config_data[3] = {0x06, 0x00, 0x70};
	chainbus_I2C_write(0x20, config_data, 3);

	// Deselect the hat to return bus selection to position 0
	chainbus_deselect_hat(position);
}

void chainbus_hat_protocol_config_write(Hat_position position, int config_pin, int state)
{
	uint8_t reg = (config_pin < 8) ? 0x02 : 0x03;
	uint8_t bit = (config_pin < 8) ? config_pin : (config_pin - 8);

	// Select the target hat position
	chainbus_select_hat(position);
	chainbus_I2C_config_speed(chainbus_I2C_config_speed_standard);

	// 1. Read the current output register value
	uint8_t val = 0;
	uint8_t write_reg[1] = {reg};
	chainbus_I2C_write_read(0x20, write_reg, 1, &val, 1);

	// 2. Modify the target pin bit
	if (state)
	{
		val |= (1 << bit);
	}
	else
	{
		val &= ~(1 << bit);
	}

	// 3. Write back the updated output register value
	uint8_t write_val[2] = {reg, val};
	chainbus_I2C_write(0x20, write_val, 2);

	// Deselect the hat to return bus selection to position 0
	chainbus_deselect_hat(position);
}

void chainbus_hat_protocol_i2c_write(Hat_position position, uint8_t addr, const uint8_t *data, size_t len)
{
	chainbus_select_hat(position);
	chainbus_I2C_config_speed(chainbus_I2C_config_speed_standard);
	chainbus_I2C_write(addr, data, len);
	chainbus_deselect_hat(position);
}

void chainbus_hat_protocol_i2c_read(Hat_position position, uint8_t addr, uint8_t *data, size_t len)
{
	chainbus_select_hat(position);
	chainbus_I2C_config_speed(chainbus_I2C_config_speed_standard);
	chainbus_I2C_read(addr, data, len);
	chainbus_deselect_hat(position);
}

void chainbus_hat_protocol_I2C_write_read(Hat_position position, uint8_t addr, uint8_t *write_data, int write_len, uint8_t *read_data, int read_len)
{
	chainbus_select_hat(position);
	chainbus_I2C_config_speed(chainbus_I2C_config_speed_standard);
	chainbus_I2C_write_read(addr, write_data, write_len, read_data, read_len);
	chainbus_deselect_hat(position);
}