#include "stm32f4xx.h" // Adjust based on the specific STM32 series
#include "core_cm4.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"
#include "IRSensor.h"
#include "uart_init.h"

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

void temp_uart(uint32_t baud);
int temp_start(void);
int temp_read(void);
void temp_sensor_check(void);
extern float temperature;
