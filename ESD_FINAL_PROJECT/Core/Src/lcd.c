#include "lcd.h"

// Define control macros for GPIO pins
#define CS_LOW()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET)
#define CS_HIGH()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET)
#define DC_COMMAND() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)
#define DC_DATA()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)
#define RST_LOW()   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET)
#define RST_HIGH()  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET)
#define SD_CS_PORT GPIOD
#define SD_CS_PIN GPIO_PIN_9

// Function to send a command
void ILI9341_SendCommand(uint8_t cmd) {
    CS_LOW();
    DC_COMMAND();
    HAL_SPI_Transmit(&hspi2, &cmd, 1, HAL_MAX_DELAY);
    CS_HIGH();
}

// Function to send data
void ILI9341_SendData(uint8_t *data, uint16_t size) {
    CS_LOW();
    DC_DATA();
    HAL_SPI_Transmit(&hspi2, data, size, HAL_MAX_DELAY);
    CS_HIGH();
}

// Function to send a single byte of data
void ILI9341_SendDataByte(uint8_t data) {
    CS_LOW();
    DC_DATA();
    HAL_SPI_Transmit(&hspi2, &data, 1, HAL_MAX_DELAY);
    CS_HIGH();
}

// Initialization sequence for ILI9341
void ILI9341_Init(void) {
    // Reset the display
    RST_LOW();
    HAL_Delay(10);  // 10ms delay
    RST_HIGH();
    HAL_Delay(120); // 120ms delay after reset

    // Initialization commands based on the datasheet
    uint8_t data[15];

    // Power Control A
    ILI9341_SendCommand(0xCB);
    data[0] = 0x39; data[1] = 0x2C; data[2] = 0x00; data[3] = 0x34; data[4] = 0x02;
    ILI9341_SendData(data, 5);

    // Power Control B
    ILI9341_SendCommand(0xCF);
    data[0] = 0x00; data[1] = 0xC1; data[2] = 0x30;
    ILI9341_SendData(data, 3);

    // Driver Timing Control A
    ILI9341_SendCommand(0xE8);
    data[0] = 0x85; data[1] = 0x00; data[2] = 0x78;
    ILI9341_SendData(data, 3);

    // Driver Timing Control B
    ILI9341_SendCommand(0xEA);
    data[0] = 0x00; data[1] = 0x00;
    ILI9341_SendData(data, 2);

    // Power On Sequence Control
    ILI9341_SendCommand(0xED);
    data[0] = 0x64; data[1] = 0x03; data[2] = 0x12; data[3] = 0x81;
    ILI9341_SendData(data, 4);

    // Pump Ratio Control
    ILI9341_SendCommand(0xF7);
    data[0] = 0x20;
    ILI9341_SendData(data, 1);

    // Power Control 1
    ILI9341_SendCommand(0xC0);
    data[0] = 0x23;
    ILI9341_SendData(data, 1);

    // Power Control 2
    ILI9341_SendCommand(0xC1);
    data[0] = 0x10;
    ILI9341_SendData(data, 1);

    // VCOM Control 1
    ILI9341_SendCommand(0xC5);
    data[0] = 0x3E; data[1] = 0x28;
    ILI9341_SendData(data, 2);

    // VCOM Control 2
    ILI9341_SendCommand(0xC7);
    data[0] = 0x86;
    ILI9341_SendData(data, 1);

    // Memory Access Control
    ILI9341_SendCommand(0x36);
    data[0] = 0x48;
    ILI9341_SendData(data, 1);

    // Pixel Format Set
    ILI9341_SendCommand(0x3A);
    data[0] = 0x55;  // 16-bit RGB565
    ILI9341_SendData(data, 1);

    // Frame Rate Control
    ILI9341_SendCommand(0xB1);
    data[0] = 0x00; data[1] = 0x18;
    ILI9341_SendData(data, 2);

    // Display Function Control
    ILI9341_SendCommand(0xB6);
    data[0] = 0x08; data[1] = 0x82; data[2] = 0x27;
    ILI9341_SendData(data, 3);

    // Exit Sleep Mode
    ILI9341_SendCommand(0x11);
    HAL_Delay(120);

    // Turn on Display
    ILI9341_SendCommand(0x29);
}

void ILI9341_FillScreen(uint16_t color) {
    uint8_t high_byte = (color >> 8) & 0xFF; // Extract high byte of RGB565 color
    uint8_t low_byte = color & 0xFF;        // Extract low byte of RGB565 color

    // Set column address (entire width of the display)
    ILI9341_SendCommand(0x2A); // Column Address Set
    uint8_t data[4] = {0x00, 0x00, 0x00, 0xEF}; // Start: 0x0000, End: 0x00EF (239 in decimal)
    ILI9341_SendData(data, 4);

    // Set page address (entire height of the display)
    ILI9341_SendCommand(0x2B); // Page Address Set
    data[0] = 0x00; data[1] = 0x00; data[2] = 0x01; data[3] = 0x3F; // Start: 0x0000, End: 0x013F (319 in decimal)
    ILI9341_SendData(data, 4);

    // Start memory write
    ILI9341_SendCommand(0x2C); // Memory Write

    // Write color to every pixel
    for (uint32_t i = 0; i < 240 * 320; i++) { // 240x320 resolution
        uint8_t color_data[2] = {high_byte, low_byte};
        ILI9341_SendData(color_data, 2);
    }
}

