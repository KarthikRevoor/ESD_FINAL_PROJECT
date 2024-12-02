/*
 * IRSensor.h
 *
 *  Created on: Nov 4, 2024
 *      Author: karth
 */

#ifndef SRC_IRSENSOR_H_
#define SRC_IRSENSOR_H_

#include "stm32f4xx.h" // Adjust based on the specific STM32 series
#include "core_cm4.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"
#include "IRSensor.h"
#include "uart_init.h"

void humidity_check(void);
uint8_t read_data(uint8_t* humidity, uint8_t* temperature);
void delay_us(uint32_t us);
void set_input(void);
void set_output(void);
void humidity_init(void);


#endif /* SRC_IRSENSOR_H_ */
