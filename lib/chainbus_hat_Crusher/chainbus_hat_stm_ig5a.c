#include "chainbus_hat_stm_ig5a.h"

/* For some reason you have to subtract 1 from the addresses given in the
 * documentation. Source: table on page 11-7 */
enum {
    IG5A_ADDR_PARAMETER_LOCK = 0x0004 - 1,
    IG5A_ADDR_FREQ_REFERENCE = 0x0005 - 1,
    IG5A_ADDR_RUN_COMMAND    = 0x0006 - 1
};

void ig5a_init(iG5A *inv, UART_HandleTypeDef *huart, uint8_t inverter_address)
{
    modbus_device_init(&inv->modbus, huart, inverter_address);
}

void ig5a_init_rs485(iG5A *inv, UART_HandleTypeDef *huart, uint8_t inverter_address,
                      GPIO_TypeDef *de_port, uint16_t de_pin)
{
    modbus_device_init_rs485(&inv->modbus, huart, inverter_address, de_port, de_pin);
}

bool ig5a_set_frequency(iG5A *inv, float f)
{
    return modbus_write_register(&inv->modbus, IG5A_ADDR_FREQ_REFERENCE,
                                  (uint16_t) (f * 100)) == MODBUS_NO_EXCEPTION;
}

bool ig5a_start(iG5A *inv, bool reverse)
{
    return modbus_write_register(&inv->modbus, IG5A_ADDR_RUN_COMMAND,
                                  reverse ? 0b100 : 0b010) == MODBUS_NO_EXCEPTION;
}

bool ig5a_stop(iG5A *inv)
{
    return modbus_write_register(&inv->modbus, IG5A_ADDR_RUN_COMMAND, 1) == MODBUS_NO_EXCEPTION;
}

bool ig5a_lock(iG5A *inv)
{
    return modbus_write_register(&inv->modbus, IG5A_ADDR_PARAMETER_LOCK, 0) == MODBUS_NO_EXCEPTION;
}

bool ig5a_unlock(iG5A *inv)
{
    return modbus_write_register(&inv->modbus, IG5A_ADDR_PARAMETER_LOCK, 1) == MODBUS_NO_EXCEPTION;
}

bool ig5a_is_locked(iG5A *inv)
{
    uint16_t value = 0;
    modbus_read_register(&inv->modbus, IG5A_ADDR_PARAMETER_LOCK, &value, false);
    return !value; /* 0 means locked */
}

bool ig5a_is_running(iG5A *inv)
{
    uint16_t value = 0;
    modbus_read_register(&inv->modbus, IG5A_ADDR_RUN_COMMAND, &value, false);
    bool forward = (value >> 1) & 1;
    bool reverse = (value >> 2) & 1;
    return forward || reverse;
}