
// ChainBus
#include "chainbus_header_user.h"
#include "chainbus_header_hat.h"
#include <chainbus_low_stm_modbus.h>
#include <chainbus_hat_stm_ig5a.h>
#include <chainbus_hat_protocol.h>
// don't include chainbus_header_hat.h
// It's supposed to be only visible for hats
// you only will need to use functions from hat library

hat_data hat_gpio_basic;
#define pin_1 1

int main()
{

	/* Initialization */

	// initialize the entire chanbus, pin modes, all bussess
	chainbus_init();
	//chainbus_delay_ms(50);
	// find position of each hat, be it by name, UUID or absoulute position
	chainbus_uni_find_hat_position(4,&hat_gpio_basic.position);
	chainbus_hat_protocol_init(hat_gpio_basic.position);
	// initialize hats (turn off everything on them, set them to safe postions and settings)


		
	    //chainbus_hat_gpio_basic_init(hat_gpio_basic.position);


	/* Main user code */

	// set pin 1 mode of hat gpio as output
	//chainbus_hat_gpio_basic_set_mode(hat_gpio_basic.position, pin_1, chainbus_hat_gpio_basic_mode_output);
	
	chainbus_hat_protocol_config_write(hat_gpio_basic.position,  CHAINBUS_PIN_UART_1_SHDN_N , 0);
 	chainbus_hat_protocol_config_write(hat_gpio_basic.position, CHAINBUS_PIN_UART_1_RE_N, 1);
	for(int i=0;i<16;i++)
	{
		chainbus_hat_protocol_config_write(hat_gpio_basic.position,  i , 1);
	}
	//chainbus_select_hat(hat_gpio_basic.position);
	// blinky!
	while (1)
	{
	}
}
