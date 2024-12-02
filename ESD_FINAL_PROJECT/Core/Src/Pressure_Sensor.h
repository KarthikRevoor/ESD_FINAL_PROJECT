#include "DS18b20.h"
#include "uart_init.h"
#include <stdio.h>
#include <stdlib.h>
#include "IRSensor.h"
// Pin definitions for the ILI9341
void ADC_Output(void);
void ADC_Read(void);
void ADC_IRQHandler(void);
void ADC_Config(void);
void Pressure_sensor_Init(void);

extern volatile uint16_t adcValue;
