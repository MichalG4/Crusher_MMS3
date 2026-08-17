#ifndef IG5A_H
#define IG5A_H

#ifdef __cplusplus
extern "C" {
#endif

#include "chainbus_low_stm_modbus.h"

/* ModbusRTU based interface for the LS iG5A inverter, based on:
 * LS_Inverter_SV-IG5A_Series_User_Manual.pdf
 * Don't use the polish manual, the translation is trash.
 *
 * To communicate with the inverter you have to set the following
 * parameters on it:
 *   Drv = 3, Frq = 7, I59 = 0, I60 = 1, I61 = 3, I62 = 0, I63 = 1
 */

/* Source: exception table on page 11-3 */
enum {
    IG5A_ILLEGAL_FUNCTION     = 0x01,
    IG5A_ILLEGAL_DATA_ADDRESS = 0x02,
    IG5A_ILLEGAL_DATA_VALUE   = 0x03,
    IG5A_SLAVE_DEVICE_BUSY    = 0x06,
    IG5A_WRITE_DISABLED       = 0x14 /* user defined, RTFM */
};

typedef struct {
    ModbusRTUDevice modbus;
} iG5A;

/* No DE pin (transceiver directions itself automatically). */
void ig5a_init(iG5A *inv, UART_HandleTypeDef *huart, uint8_t inverter_address);

/* With a manually-driven RS485 DE/~RE pin. */
void ig5a_init_rs485(iG5A *inv, UART_HandleTypeDef *huart, uint8_t inverter_address,
                      GPIO_TypeDef *de_port, uint16_t de_pin);

bool ig5a_set_frequency(iG5A *inv, float f);
bool ig5a_start(iG5A *inv, bool reverse);
bool ig5a_stop(iG5A *inv);
bool ig5a_lock(iG5A *inv);
bool ig5a_unlock(iG5A *inv);
bool ig5a_is_locked(iG5A *inv);
bool ig5a_is_running(iG5A *inv);

#ifdef __cplusplus
}
#endif

#endif /* IG5A_H */