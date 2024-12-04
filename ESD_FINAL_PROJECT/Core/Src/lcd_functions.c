#include "lcd_functions.h"
#include "fatfs.h"
#include "gfxfont.h"


void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if ((x >= 240) || (y >= 320)) return; // Ensure bounds

    ILI9341_SendCommand(0x2A); // Column Address Set
    uint8_t data_x[] = {(x >> 8) & 0xFF, x & 0xFF, (x >> 8) & 0xFF, x & 0xFF};
    ILI9341_SendData(data_x, 4);

    ILI9341_SendCommand(0x2B); // Page Address Set
    uint8_t data_y[] = {(y >> 8) & 0xFF, y & 0xFF, (y >> 8) & 0xFF, y & 0xFF};
    ILI9341_SendData(data_y, 4);

    ILI9341_SendCommand(0x2C); // Memory Write
    uint8_t color_data[] = {(color >> 8) & 0xFF, color & 0xFF};
    ILI9341_SendData(color_data, 2);
}
void DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t scale) {
    if (c < 0 || c > 127) return; // Ensure the character is within the supported range

    // Map ASCII character to the font array
    const unsigned char *charBitmap = &font[c * 5];

    // Draw each column of the character
    for (uint8_t col = 0; col < 5; col++) { // Each character is 5 columns wide
        uint8_t columnData = charBitmap[col];
        for (uint8_t row = 0; row < 8; row++) { // Each column has 8 rows
            if (columnData & (1 << row)) { // Check if pixel is ON
                // Draw a scaled block for the ON pixel
                for (uint8_t i = 0; i < scale; i++) {
                    for (uint8_t j = 0; j < scale; j++) {
                        ILI9341_DrawPixel(x + col * scale + i, y + row * scale + j, color);
                    }
                }
            } else {
                // Draw a scaled block for the OFF pixel (background)
                for (uint8_t i = 0; i < scale; i++) {
                    for (uint8_t j = 0; j < scale; j++) {
                        ILI9341_DrawPixel(x + col * scale + i, y + row * scale + j, bg);
                    }
                }
            }
        }
    }

    // Add scaled spacing after each character
    for (uint8_t row = 0; row < 8 * scale; row++) {
        for (uint8_t i = 0; i < scale; i++) {
            ILI9341_DrawPixel(x + 5 * scale + i, y + row, bg);
        }
    }
}


void DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t scale) {
    while (*str) {
        DrawChar(x, y, *str, color, bg, scale);
        x += 6 * scale; // Adjust character spacing based on scale
        str++;
    }
}

void DrawImage(uint16_t x, uint16_t y, const uint16_t *image, uint16_t width, uint16_t height) {
    // Set the drawing window
    ILI9341_SendCommand(0x2A); // Column Address Set
    uint8_t colData[] = {x >> 8, x & 0xFF, (x + width - 1) >> 8, (x + width - 1) & 0xFF};
    ILI9341_SendData(colData, 4);

    ILI9341_SendCommand(0x2B); // Page Address Set
    uint8_t rowData[] = {y >> 8, y & 0xFF, (y + height - 1) >> 8, (y + height - 1) & 0xFF};
    ILI9341_SendData(rowData, 4);

    // Write memory
    ILI9341_SendCommand(0x2C); // Memory Write
    ILI9341_SendData((uint8_t *)image, width * height * 2); // Send pixel data
}



void ILI9341_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    // Ensure the rectangle lies within the screen boundaries
    if ((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;

    // Clip width and height if the rectangle extends beyond screen boundaries
    if ((x + width - 1) >= ILI9341_WIDTH) {
        width = ILI9341_WIDTH - x;
    }
    if ((y + height - 1) >= ILI9341_HEIGHT) {
        height = ILI9341_HEIGHT - y;
    }

    // Set column address
    ILI9341_SendCommand(0x2A); // Column Address Set
    uint8_t colData[4] = { (x >> 8) & 0xFF, x & 0xFF, ((x + width - 1) >> 8) & 0xFF, (x + width - 1) & 0xFF };
    ILI9341_SendData(colData, 4);

    // Set row address
    ILI9341_SendCommand(0x2B); // Page Address Set
    uint8_t rowData[4] = { (y >> 8) & 0xFF, y & 0xFF, ((y + height - 1) >> 8) & 0xFF, (y + height - 1) & 0xFF };
    ILI9341_SendData(rowData, 4);

    // Write memory
    ILI9341_SendCommand(0x2C); // Memory Write

    // Fill the rectangle with color
    uint16_t pixelCount = width * height;
    uint8_t colorData[2] = { (color >> 8) & 0xFF, color & 0xFF }; // Split color into 2 bytes
    for (uint16_t i = 0; i < pixelCount; i++) {
        ILI9341_SendData(colorData, 2);
    }
}

void DisplayBitmapFromSD(const char *filename) {
    FIL file;
    FRESULT result;
    uint16_t imageWidth, imageHeight;
    uint32_t dataOffset;
    uint8_t bmpHeader[54];  // BMP header is 54 bytes
    uint16_t color;

    // Open the bitmap file
    result = f_open(&file, filename, FA_READ);
    if (result != FR_OK) {
        printf("Error: Failed to open file %s\n", filename);
        return;
    }

    // Read the BMP header
    UINT bytesRead;
    f_read(&file, bmpHeader, sizeof(bmpHeader), &bytesRead);
    if (bytesRead != sizeof(bmpHeader)) {
        printf("Error: Failed to read BMP header from %s\n", filename);
        f_close(&file);
        return;
    }

    // Extract width, height, and pixel data offset
    imageWidth = *(uint16_t *)&bmpHeader[18];
    imageHeight = *(uint16_t *)&bmpHeader[22];
    dataOffset = *(uint32_t *)&bmpHeader[10];

    // Check for valid image dimensions
    if (imageWidth > ILI9341_WIDTH || imageHeight > ILI9341_HEIGHT) {
        printf("Error: Image size (%dx%d) exceeds screen dimensions\n", imageWidth, imageHeight);
        f_close(&file);
        return;
    }

    printf("Info: Image size %dx%d, Pixel data offset: %lu\n", imageWidth, imageHeight, dataOffset);

    // Move to the pixel data
    f_lseek(&file, dataOffset);

    // Read and display the image pixel data
    for (uint16_t y = 0; y < imageHeight; y++) {
        for (uint16_t x = 0; x < imageWidth; x++) {
            uint8_t pixelData[3];  // 24-bit RGB
            f_read(&file, pixelData, 3, &bytesRead);
            if (bytesRead != 3) {
                printf("Error: Failed to read pixel data\n");
                f_close(&file);
                return;
            }

            // Convert RGB888 to RGB565
            color = ((pixelData[2] & 0xF8) << 8) | ((pixelData[1] & 0xFC) << 3) | (pixelData[0] >> 3);

            // Draw the pixel
            ILI9341_DrawPixel(x, imageHeight - 1 - y, color); // BMP starts from the bottom-left
        }
    }

    printf("Info: Image %s displayed successfully\n", filename);
    f_close(&file);
}
