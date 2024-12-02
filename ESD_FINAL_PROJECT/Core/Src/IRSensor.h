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
extern volatile uint32_t ir_triggered;
// Initialize the IR sensor pin with interrupt
void IR_init(void);

// EXTI interrupt callback for handling IR sensor events
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

// Optional: Check if the IR sensor has been triggered
uint8_t IR_is_triggered(void);
void EXTI9_5_IRQHandler(void) ;




#endif /* SRC_IRSENSOR_H_ */
