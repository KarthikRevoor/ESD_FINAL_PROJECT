/*
 * Handle_SD_Card.h
 *
 *  Created on: Dec 4, 2024
 *      Author: karth
 */

#ifndef SRC_HANDLE_SD_CARD_H_
#define SRC_HANDLE_SD_CARD_H_

#include "fatfs.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include "uart_init.h"
#include "FATFS_SD.h"
#include "stdbool.h"
#include "lcd_functions.h"


void process_SD_card( const char *filename );
void display_images(void);
void SD_Deinit(void);
void SD_Init(void);
void DrawPokeball(int x, int y);
#endif /* SRC_HANDLE_SD_CARD_H_ */
