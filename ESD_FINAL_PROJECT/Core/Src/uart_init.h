#include "stm32f4xx.h" // Adjust based on the specific STM32 series
#include "core_cm4.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"

void uart_send_string(const char *str);
void pin_init(void);
