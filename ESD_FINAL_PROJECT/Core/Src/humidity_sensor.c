#include "humidity_sensor.h"

volatile uint32_t idr_pin_state;
volatile uint32_t odr_pin_state;
void humidity_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	    // Set PC5 as output initially
	    GPIOC->MODER &= ~(3 << (6 * 2));
	    GPIOC->MODER |= (1 << (6 * 2));

	    // Enable internal pull-up for PC6
	    GPIOC->PUPDR &= ~(3 << (6 * 2));  // Clear pull-up/pull-down bits
	    GPIOC->PUPDR |= (1 << (6 * 2));   // Set pull-up mode
}

void set_input(void)
{
	GPIOC->MODER &= ~(3<<(6*2));
}

void set_output(void){
    GPIOC->MODER &= ~(3 << (6 * 2));
    GPIOC->MODER |= (1 << (6 * 2));
}

void delay_us(uint32_t us)
{
    // Use HAL library delay, which has microsecond precision
    HAL_Delay(us);
}
uint8_t read_data(uint8_t* humidity, uint8_t* temperature)
{
uint8_t data[5]= {0};
uint8_t i,j;

set_output();
GPIOC->ODR &=~(1<<6);
for (volatile int i=0; i<18000; i++)
GPIOC->ODR |=(1<<6);
delay_us(30);

set_input();

if((GPIOC->IDR & (1<<6))!=0)
	return 1;
delay_us(80);
if((GPIOC->IDR & (1<<6))==0)
	return 1;
delay_us(80);

for (j = 0; j < 8; j++) {  // For each byte
        for (i = 0; i < 8; i++) {  // For each bit
            while ((GPIOC->IDR & (1 << 6)) == 0);  // Wait for low signal
            delay_us(40);  // 40us delay to differentiate 0 and 1
            if (GPIOC->IDR & (1 << 6)) {
                data[j] |= (1 << (7 - i));  // Set bit if signal high
                while ((GPIOC->IDR & (1 << 6)) != 0);  // Wait for signal to go low
            }
        }
    }

set_output();

if(data[4]== (data[0]+data[1]+data[2]+data[3]))
{
	*humidity = data[0];
	*temperature= data[2];
	return 0;
}
else
{
	return 2;
}
}

//void humidity_check(void)
//{
//    uint8_t humidity, temperature;
//    char buffer[50];
//	if (read_data(&humidity,&temperature)==0)
//	{snprintf(buffer, sizeof(buffer), "Humidity: %d%%, Temperature: %dC\r\n", humidity, temperature);
//    uart_send_string(buffer);
//    } else {
//          uart_send_string("DHT11 Error\r\n");
//      }
//      for (volatile int i = 0; i < 2000000; i++);
//  }

