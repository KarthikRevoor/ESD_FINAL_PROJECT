/*
 * lcd_functions.h
 *
 *  Created on: Dec 3, 2024
 *      Author: karth
 */

#ifndef SRC_LCD_FUNCTIONS_H_
#define SRC_LCD_FUNCTIONS_H_
#include "lcd.h"
#include "fonts.h"
#include "gfxfont.h"
#define ILI9341_WIDTH 240
#define ILI9341_HEIGHT 320
void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t scale);
void DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t scale);
void DrawImage(uint16_t x, uint16_t y, const uint16_t *image, uint16_t width, uint16_t height);
void ILI9341_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void DisplayBitmapFromSD(const char *filename);
#endif /* SRC_LCD_FUNCTIONS_H_ */
