#include "Handle_SD_Card.h"
#include "lcd.h" // Include your LCD library for drawing

#define POKEBALL_WIDTH 15
#define POKEBALL_HEIGHT 15
static FATFS FatFs; // Persistent FatFS handle

// Initialize the SD card once
void SD_Init(void) {
    FRESULT fres = f_mount(&FatFs, "", 1); // Mount the SD card
    if (fres != FR_OK) {
        printf("Error: Failed to mount SD card (%i)\r\n", fres);
    } else {
        printf("SD Card Mounted Successfully!\r\n");
    }
}

// Unmount the SD card when the program exits
void SD_Deinit(void) {
    f_mount(NULL, "", 0); // Unmount the SD card
    printf("SD Card Unmounted Successfully!\r\n");
}

void process_SD_card(const char *filename) {
    FIL fil;                 // File handle
    FRESULT fres;            // Result after operations

    do {
        // Open the BMP file
        fres = f_open(&fil, filename, FA_READ);
        if (fres != FR_OK) {
            printf("Error: Failed to open file %s (%i)\r\n", filename, fres);
            break;
        }

        printf("File %s opened successfully!\r\n", filename);

        // Read and parse BMP header
        uint8_t bmpHeader[54];
        UINT bytesRead;
        fres = f_read(&fil, bmpHeader, sizeof(bmpHeader), &bytesRead);
        if (fres != FR_OK || bytesRead != sizeof(bmpHeader)) {
            printf("Error: Failed to read BMP header\r\n");
            f_close(&fil);
            break;
        }

        // Extract BMP information
        uint16_t imageWidth = *(uint16_t *)&bmpHeader[18];
        uint16_t imageHeight = *(uint16_t *)&bmpHeader[22];
        uint32_t dataOffset = *(uint32_t *)&bmpHeader[10];
        uint16_t bpp = *(uint16_t *)&bmpHeader[28]; // Bits per pixel
        printf("Image Info: Width=%d, Height=%d, DataOffset=%lu, BPP=%d\r\n",
               imageWidth, imageHeight, dataOffset, bpp);

        // Validate BMP format
        if (bpp != 24 && bpp != 16) { // Check for 24-bit or 16-bit BMP
            printf("Error: Unsupported BMP format. Only 24-bit and 16-bit supported.\r\n");
            f_close(&fil);
            break;
        }

        // Move to pixel data
        if (f_lseek(&fil, dataOffset) != FR_OK) {
            printf("Error: Failed to seek to pixel data\r\n");
            f_close(&fil);
            break;
        }

        printf("Starting image rendering...\r\n");

        uint32_t rowSize = ((imageWidth * (bpp / 8) + 3) & ~3); // Ensure 4-byte alignment
        uint16_t maxRowsPerBuffer = 128; // Increased rows per buffer
        uint32_t bufferSize = imageWidth * maxRowsPerBuffer * 2; // RGB565 buffer size
        uint8_t rowBuffer[rowSize]; // Buffer for a single row
        uint8_t lineBuffer[bufferSize]; // Buffer for multiple rows

        ILI9341_SetAddressWindow(0, 0, imageWidth - 1, imageHeight - 1);

        for (uint16_t y = 0; y < imageHeight; y += maxRowsPerBuffer) {
            uint16_t rowsToProcess = (y + maxRowsPerBuffer > imageHeight)
                                     ? (imageHeight - y)
                                     : maxRowsPerBuffer;
            uint8_t *linePtr = lineBuffer;

            for (uint16_t row = 0; row < rowsToProcess; row++) {
                fres = f_read(&fil, rowBuffer, rowSize, &bytesRead);
                if (fres != FR_OK || bytesRead < rowSize) {
                    printf("Error: Failed to read row %d, fres=%d, bytesRead=%u\r\n", y + row, fres, bytesRead);
                    break;
                }

                for (uint16_t x = 0; x < imageWidth; x++) {
                    uint16_t color;

                    if (bpp == 24) {
                        // 24-bit RGB (RGB888 -> RGB565)
                        uint8_t *pixel = &rowBuffer[x * 3];
                        color = ((pixel[2] & 0xF8) << 8) | // Red
                                ((pixel[1] & 0xFC) << 3) | // Green
                                (pixel[0] >> 3);          // Blue
                    } else if (bpp == 16) {
                        // 16-bit RGB565
                        uint16_t *pixel = (uint16_t *)&rowBuffer[x * 2];
                        color = *pixel; // Directly use the RGB565 value
                    }

                    *linePtr++ = color >> 8;        // High byte
                    *linePtr++ = color & 0xFF;     // Low byte
                }
            }

            // Send the entire buffer for rowsToProcess
            ILI9341_SendData(lineBuffer, rowsToProcess * imageWidth * 2);
        }

        printf("Image rendered successfully!\r\n");

        // Close the BMP file
        f_close(&fil);
        printf("File %s closed successfully!\r\n", filename);

    } while (false);

}

// Example usage
void display_images(void)
{
    process_SD_card("tiger.bmp");
    process_SD_card("easy.bmp");
}




// Pokeball sprite data (RGB565 color values)
const uint16_t pokeball_sprite[POKEBALL_HEIGHT][POKEBALL_WIDTH] = {
    {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF}, // Top row
    {0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF},
    {0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF},
    {0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0xFFFF, 0xFFFF},
    {0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xF800, 0xF800, 0xFFFF},
    {0xFFFF, 0xF800, 0xF800, 0xF800, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xF800, 0xFFFF},
    {0xF800, 0xF800, 0xF800, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xF800},
    {0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000},
    {0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF},
    {0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF},
    {0xFFFF, 0xF800, 0xF800, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xF800, 0xF800, 0xFFFF},
    {0xFFFF, 0xF800, 0xF800, 0xF800, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xF800, 0xFFFF, 0xFFFF},
    {0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF},
    {0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF},
    {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF}
};

// Function to draw the Pokeball sprite
void DrawPokeball(int x, int y) {
    for (int i = 0; i < POKEBALL_HEIGHT; i++) {
        for (int j = 0; j < POKEBALL_WIDTH; j++) {
            ILI9341_DrawPixel(x + j, y + i, pokeball_sprite[i][j]);
        }
    }
}
