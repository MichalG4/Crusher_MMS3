#include "iG5A.h"
#include "chainbus_header_hat.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// For some reason you have to subtract 1 from addresses in documentation
// Source: table on page 11-7
#define ParameterLock 0x0003 //(0x0004 - 1),
#define FreqReference 0x0004 //(0x0005 - 1),
#define RunCommand 0x0005    //(0x0006 - 1),

#define POST_FRAME_DELAY 25

// source: https://www.modbustools.com/modbus.html#CRC
uint16_t CRC16(const uint8_t *data, uint16_t length)
{
    static const uint16_t CRC_table[] = {
        0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
        0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
        0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
        0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
        0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
        0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
        0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
        0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
        0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
        0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
        0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
        0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
        0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
        0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
        0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
        0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
        0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
        0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
        0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
        0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
        0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
        0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
        0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
        0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
        0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
        0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
        0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
        0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
        0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
        0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
        0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
        0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040};

    uint8_t temp;
    uint16_t CRC = 0xFFFF;

    while (length--)
    {
        temp = *data++ ^ CRC;
        CRC >>= 8;
        CRC ^= CRC_table[temp];
    }
    return CRC;
}

bool verify_CRC16(const uint8_t *data, uint16_t length)
{
    uint16_t receivedCRC;
    receivedCRC |= (uint16_t)data[length - 1] << 8;
    receivedCRC |= (uint16_t)data[length - 2];
    return CRC16(data, length - 2) == receivedCRC;
}

void send_frame(const uint8_t *frame, uint8_t length);
{
    (void)chainbus_UART_send(*frame, (int32_t)length);
}
void receive_frame(uint8_t *frame, uint8_t length);
{
    chainbus_UART_read_buffer(*frame, (int32_t)length);
}
uint8_t count_received_bytes(uint8_t *frame, uint8_t length)
{
    int32_t BytesCount = 0;
    chainbus_UART_read_buffer_how_many_bytes(&BytesCount);
    return (uint8_t)BytesCount;
}
void clear_input_buffer(void);
{
    chainbus_UART_clear_read_buffer();
}
iG5A_init(iG5A *dev, uint8_t slave_addr)
{
    dev->slave_address = slave_addr;
}
read_register(iG5A *dev, uint16_t address, uint16_t *input, iG5A_reg_access_t access)
{
    const uint8_t function_code = access ? ReadInputRegisters : ReadHoldRegisters;
    const uint16_t length = 1;
    uint8_t buffer[8];

    buffer[0] = dev->slave_address;
    buffer[1] = function_code;
    buffer[2] = (uint8_t)(address >> 8);
    buffer[3] = (uint8_t)address;
    buffer[4] = (uint8_t)(length >> 8);
    buffer[5] = (uint8_t)length;
    const uint16_t CRC = CRC16(buffer, 6);
    buffer[6] = (uint8_t)CRC;
    buffer[7] = (uint8_t)(CRC >> 8);
    clear_input_buffer();
    send_frame(buffer, 8);
    chainbus_delay_ms(POST_FRAME_DELAY); // wait for response
    const uint16_t expected_response_length = 5 + 2 * length;
    const uint16_t response_length = count_received_bytes(buffer, expected_response_length);
    chainbus_UART_read_buffer(buffer, expected_response_length);

    if (!(verify_CRC16(buffer, response_length)))
    {
        return InvalidCRC;
    }

    if (function_code != buffer[1])
    {
        return buffer[2]; // exception code
    }

    uint16_t result = 0;
    result |= (uint16_t)buffer[3] << 8;
    result |= (uint16_t)buffer[4];
    *input = result;

    return NoException;
}
exception_code_t write_register(iG5A *dev, uint16_t address, uint16_t value)
{
    const uint8_t frame_length = 8;
    uint8_t request[frame_length];
    request[0] = dev->slave_address;
    request[1] = WriteSingleRegister;
    request[2] = (uint8_t)(address >> 8);
    request[3] = (uint8_t)address;
    request[4] = (uint8_t)(value >> 8);
    request[5] = (uint8_t)value;
    const uint16_t CRC = CRC16(request, 6);
    request[6] = (uint8_t)CRC;
    request[7] = (uint8_t)(CRC >> 8);
    clear_input_buffer();
    send_frame(request, frame_length);
    chainbus_delay_ms(POST_FRAME_DELAY); // wait for response

    uint8_t response[frame_length];
    memset(response, 0, sizeof(response));

    const uint16_t response_length = count_received_bytes(response, frame_length);
    chainbus_UART_read_buffer(response, frame_length);

    if (!verify_CRC16(response, response_length))
    {
        return InvalidCRC;
    }

    if (memcmp(request, response, frame_length))
    {
        return response[2]; // exception code
    }
    return NoException;
}

void serial_begin(uint32_t BaudRate)
{
    chainbus_UART_config_t newConfig = {BaudRate, 8, chainbus_UART_config_stop_bits_1, chainbus_UART_config_parity_none};
    chainbus_UART_config(newConfig); // i think thats all??
}

bool set_frequency(iG5A *dev, float f)
{
    uint16_t raw_value = (uint16_t)(f * 100.0f);
    return write_register(dev, FreqReference, raw_value);
}

bool start(iG5A *dev, bool reverse)
{
    return write_register(dev, RunCommand, (reverse ? 0b100 : 0b010));
}

bool stop(iG5A *dev)
{
    return write_register(dev, RunCommand, 1);
}

bool lock(iG5A *dev)
{
    return write_register(dev, ParameterLock, 0);
}

bool unlock(iG5A *dev)
{
    return write_register(dev, ParameterLock, 1);
}

bool is_locked(iG5A *dev)
{
    uint16_t value;
    read_register(dev, ParameterLock, &value, REG_READ_NORMAL);
    return !value; // 0 means locked
}

bool is_running(iG5A *dev)
{
    uint16_t value;
    read_register(RunCommand, &value);
    bool forward = (value >> 1) & 1;
    bool reverse = (value >> 2) & 1;
    return forward || reverse;
}
