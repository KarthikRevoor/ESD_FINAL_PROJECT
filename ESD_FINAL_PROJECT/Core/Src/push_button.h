/*
 * push_button.h
 *
 *  Created on: Dec 2, 2024
 *      Author: karth
 */

#ifndef SRC_PUSH_BUTTON_H_
#define SRC_PUSH_BUTTON_H_

#include "stm32f4xx_hal.h"
void Push_Button_Init(void);
uint8_t Get_Button_State(void);
void Reset_Button_State(void);

#endif /* SRC_PUSH_BUTTON_H_ */
