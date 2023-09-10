#include"../1-MCAL/UART/UART_Interface.h"
#define F_CPU 16000000UL
#include<util/delay.h>
int main(void)
{
	UART_enu_Init();
	while(1){
		UART_enu_sendString("EM");
		_delay_ms(500);
		UART_enu_sendNUmber(15);
		_delay_ms(500);
		UART_enu_sendNUmber(30);
		_delay_ms(500);
		UART_enu_sendNUmber(45);
		_delay_ms(500);
		UART_enu_sendNUmber(55);
		_delay_ms(500);
		UART_enu_sendString("SM");
		_delay_ms(500);
	}

	return 0;
}
