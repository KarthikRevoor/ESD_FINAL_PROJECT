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
#include "Handle_SD_Card.h"
#include "stdbool.h"
#define ILI9341_WIDTH 240
#define ILI9341_HEIGHT 320
void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t scale);
void DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t scale);
void DrawImage(uint16_t x, uint16_t y, const uint16_t *image, uint16_t width, uint16_t height);
void ILI9341_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void display_capture_screen();
void display_battle_screen(const char *pokemon_name);
void fade_background(uint16_t start_color, uint16_t end_color, int steps);

void slide_text(const char *text, uint16_t color, uint16_t bg_color, int y_pos);

void progress_bar(int x, int y, int width, int height, uint16_t fill_color, uint16_t bg_color, int percentage);

void shake_element(int x, int y, int width, int height, uint16_t color);

void circular_expansion(int x_center, int y_center, uint16_t start_radius, uint16_t end_radius, uint16_t color);

void draw_circle(int x_center, int y_center, int radius, uint16_t color);
void update_progress_bar(int x, int y, int width, int height, uint16_t fill_color, uint16_t bg_color, int steps);
void ILI9341_DrawRect(int x, int y, int width, int height, uint16_t color);
void EncounterAnimation();
void FadeEncounterText();
void AnimatePokemonAppearance(const char *pokemon_name);
void ZoomPokemonAppearance(const char *pokemon_name);
void FlashScreen(int flashes, int duration);
void FadeTransition(uint16_t start_color, uint16_t end_color, int steps);
void FadeEncounterTextNonBlocking();
void DisplayHeadingWithTransition();

void DrawRotatedString(int x, int y, const char *text, uint16_t color, uint16_t bgcolor, int size);

void DrawRotatedChar(int x, int y, char c, uint16_t color, uint16_t bgcolor, int size);

void DisplayPokemonImage(const char *pokemon_name);
extern bool FadeTextComplete;
#endif /* SRC_LCD_FUNCTIONS_H_ */
