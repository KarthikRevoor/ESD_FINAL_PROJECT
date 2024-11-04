#include "IRSensor.h"
#include "uart_init.h"

volatile uint32_t pin_state;
void IR_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER &=~(3<<(7*2));
}

void IR_val (void)
{
pin_state= GPIOA->IDR & (1<<7);
}

void IR_check(void){
	IR_val();
if(!pin_state)
{uart_send_string("detected\r\n");
}
else
	uart_send_string("no\n\r");
}
