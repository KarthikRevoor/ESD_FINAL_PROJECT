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

void IR_check(void);
void IR_init(void);


#endif /* SRC_IRSENSOR_H_ */
