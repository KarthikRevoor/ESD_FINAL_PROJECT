#include "lcd_functions.h"
#include "fatfs.h"
#include "gfxfont.h"

bool FadeTextComplete = false;
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

void ILI9341_DrawVLine(int x, int y, int length, uint16_t color) {
    for (int i = 0; i < length; i++) {
        ILI9341_DrawPixel(x, y + i, color);
    }
}

void ILI9341_DrawHLine(int x, int y, int length, uint16_t color) {
    for (int i = 0; i < length; i++) {
        ILI9341_DrawPixel(x + i, y, color);
    }
}

void ILI9341_DrawRect(int x, int y, int width, int height, uint16_t color) {
    // Draw top horizontal line
    ILI9341_DrawHLine(x, y, width, color);

    // Draw bottom horizontal line
    ILI9341_DrawHLine(x, y + height - 1, width, color);

    // Draw left vertical line
    ILI9341_DrawVLine(x, y, height, color);

    // Draw right vertical line
    ILI9341_DrawVLine(x + width - 1, y, height, color);
}
// Add this function to display in battle state
void display_battle_screen(const char *pokemon_name) {
    ILI9341_FillScreen(0x0000); // Clear the screen with black
    ILI9341_FillRect(0, 0, 240, 50, 0xF800); // Draw a red rectangle for the header
    DrawString(10, 10, "BATTLE MODE", 0xFFFF, 0xF800, 2); // Draw "BATTLE MODE" in white

    char buffer[100];
    sprintf(buffer, "Fighting: %s", pokemon_name);
    DrawString(10, 60, buffer, 0x07E0, 0x0000, 1); // Draw Pokémon name in green
}

// Add this function to display during capture state
void display_capture_screen() {
    ILI9341_FillScreen(0x0000); // Clear the screen with black
    DrawString(10, 10, "Capture Mode", 0xFFFF, 0x0000, 2); // Title in white

    DrawString(10, 100, "Apply pressure...", 0xF800, 0x0000, 1); // Red text
    DrawString(10, 120, "3s countdown begins!", 0x07E0, 0x0000, 1); // Green text
}
void fade_background(uint16_t start_color, uint16_t end_color, int steps) {
    uint16_t current_color;
    for (int i = 0; i <= steps; i++) {
        current_color = (start_color * (steps - i) + end_color * i) / steps;
        ILI9341_FillScreen(current_color);

        // Reduce delay for smoother but quicker transition
        HAL_Delay(5); // Adjust this to balance speed and smoothness
    }
}

void slide_text(const char *text, uint16_t color, uint16_t bg_color, int y_pos) {
    for (int x = 240; x >= -strlen(text) * 8; x--) { // Move from right to left
        ILI9341_FillRect(0, y_pos, 240, 16, bg_color); // Clear previous position
        DrawString(x, y_pos, text, color, bg_color, 1); // Draw text at new position
        HAL_Delay(50); // Adjust speed of sliding
    }
}

void progress_bar(int x, int y, int width, int height, uint16_t fill_color, uint16_t bg_color, int percentage) {
    int progress_width = (width * percentage) / 100;

    // Draw only the progress portion
    ILI9341_FillRect(x, y, progress_width, height, fill_color);

    // Optionally draw the remaining portion (if needed)
    if (percentage < 100) {
        ILI9341_FillRect(x + progress_width, y, width - progress_width, height, bg_color);
    }

    // Draw a border if necessary
    ILI9341_DrawRect(x, y, width, height, 0xFFFF);
}

void update_progress_bar(int x, int y, int width, int height, uint16_t fill_color, uint16_t bg_color, int steps) {
    for (int i = 0; i <= steps; i++) {
        progress_bar(x, y, width, height, fill_color, bg_color, (i * 100) / steps);
        HAL_Delay(10); // Adjust this for smoother updates
    }
}
void shake_element(int x, int y, int width, int height, uint16_t color) {
    for (int i = 0; i < 10; i++) {
        ILI9341_FillRect(x + (i % 2 == 0 ? 2 : -2), y, width, height, color); // Move element
        HAL_Delay(50); // Delay for the shaking effect
    }
}
/**
 * @brief Draws a circle on the LCD screen.
 * @param x_center The x-coordinate of the circle's center.
 * @param y_center The y-coordinate of the circle's center.
 * @param radius The radius of the circle.
 * @param color The color of the circle (16-bit RGB565 format).
 */
void draw_circle(int x_center, int y_center, int radius, uint16_t color) {
    int x = 0;
    int y = radius;
    int decision = 1 - radius; // Decision variable

    while (x <= y) {
        // Draw all symmetric points of the circle
        ILI9341_DrawPixel(x_center + x, y_center + y, color);
        ILI9341_DrawPixel(x_center - x, y_center + y, color);
        ILI9341_DrawPixel(x_center + x, y_center - y, color);
        ILI9341_DrawPixel(x_center - x, y_center - y, color);
        ILI9341_DrawPixel(x_center + y, y_center + x, color);
        ILI9341_DrawPixel(x_center - y, y_center + x, color);
        ILI9341_DrawPixel(x_center + y, y_center - x, color);
        ILI9341_DrawPixel(x_center - y, y_center - x, color);

        x++;

        if (decision <= 0) {
            decision += 2 * x + 1;
        } else {
            y--;
            decision += 2 * (x - y) + 1;
        }
    }
}

void FlashScreen(int flashes, int duration)
{
    for (int i = 0; i < flashes; i++)
    {
        ILI9341_FillScreen(0xFFFF); // White screen
        HAL_Delay(duration-(duration/2));
        ILI9341_FillScreen(0xF7BE); // Black screen
        HAL_Delay(duration);
    }
}
void DrawRotatedString(int x, int y, const char *text, uint16_t color, uint16_t bgcolor, int size) {
    while (*text) {
        DrawRotatedChar(x, y, *text, color, bgcolor, size);
        y += size * 8; // Move y-coordinate down for next character
        text++;
    }
}

void DrawRotatedChar(int x, int y, char c, uint16_t color, uint16_t bgcolor, int size) {
    uint8_t line;
    for (int8_t i = 0; i < 5; i++) { // 5 columns per character
        line = font[c * 5 + i];
        // Reverse the bit order for the current column
        uint8_t reversed_line = 0;
        for (int8_t j = 0; j < 8; j++) {
            if (line & (1 << j)) {
                reversed_line |= (1 << (7 - j));
            }
        }

        // Draw the reversed column vertically
        for (int8_t j = 0; j < 8; j++) { // 8 rows per column
            if (reversed_line & (1 << j)) { // Now in correct order
                ILI9341_FillRect(x + j * size, y - i * size, size, size, color); // Adjust coordinates for vertical text
            } else {
                ILI9341_FillRect(x + j * size, y - i * size, size, size, bgcolor);
            }
        }
    }
}

