/*
 * lcd.h
 *
 *  Created on: Dec 3, 2024
 *      Author: karth
 */

#ifndef SRC_LCD_H_
#define SRC_LCD_H_

#include "stm32f4xx.h" // Adjust based on the specific STM32 series
#include "core_cm4.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"
#include "fatfs.h"
#include "lcd_functions.h"

void ILI9341_Init(void);
void ILI9341_SendCommand(uint8_t cmd);
void ILI9341_SendData(uint8_t *data, uint16_t size);
void ILI9341_Test(void);
void ILI9341_FillScreen(uint16_t color);
void ILI9341_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
extern SPI_HandleTypeDef hspi2;
#endif /* SRC_LCD_H_ */
