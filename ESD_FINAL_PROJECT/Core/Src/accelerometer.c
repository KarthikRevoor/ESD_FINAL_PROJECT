#include "accelerometer.h"
#include <main.h>
#include "math.h"
extern I2C_HandleTypeDef hi2c1;

void MPU_INT_Pin_Init(void) {
    // 1. Enable GPIOB clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enable GPIOB clock

    // 2. Configure PB7 as input mode
    GPIOB->MODER &= ~GPIO_MODER_MODER7; // Clear MODER7 (input mode by default)

    // 3. Configure PB7 as no pull-up/pull-down
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD7;  // Clear PUPD7 (no pull-up, no pull-down)

    // 4. Enable SYSCFG clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable SYSCFG clock

    // 5. Configure SYSCFG to route PB7 to EXTI Line 7
    SYSCFG->EXTICR[1] &= ~SYSCFG_EXTICR2_EXTI7; // Clear EXTI7 bits
    SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI7_PB; // Route PB7 to EXTI Line 7

    // 6. Configure EXTI Line 7
    EXTI->IMR |= EXTI_IMR_IM7;        // Unmask EXTI Line 7
    EXTI->RTSR |= EXTI_RTSR_TR7;      // Enable rising-edge trigger

    // 7. Enable NVIC for EXTI Line 7 (EXTI9_5 interrupt vector)
    NVIC_SetPriority(EXTI9_5_IRQn, 2); // Set priority level
    NVIC_EnableIRQ(EXTI9_5_IRQn);      // Enable EXTI9_5 interrupt
}

// EXTI interrupt handler for PB7 (EXTI Line 7)
void handle_accelerometer_interrupt(void) {
    if (EXTI->PR & EXTI_PR_PR7) {  // Check if EXTI Line 7 triggered
        EXTI->PR |= EXTI_PR_PR7;   // Clear pending interrupt

        // Read and process accelerometer data
        int32_t magnitude = MPU_read();
        const int threshold = 2000;

        if (magnitude > threshold) {
            // Increment shake count or take action
            uart_send_string("Shake detected via interrupt!\n\r");
        }
    }
}

void MPU_init(void)
{
HAL_StatusTypeDef ret = HAL_I2C_IsDeviceReady(&hi2c1, (DEV_ADD <<1)+0, 1, 100);
  if(ret ==HAL_OK)
  {
	  uart_send_string("ready\n\r");
  }
  else
	  uart_send_string("not ready\n\r");
  /* USER CODE END 2 */
  uint8_t temp_data = FS_GYRO_500;
ret = HAL_I2C_Mem_Write(&hi2c1, (DEV_ADD <<1)+0, REG_CONFIG_GYRO, 1, &temp_data, 1, 100);
  if(ret ==HAL_OK)
  {
	  uart_send_string("config gyro\n\r");
  }
  else
	  uart_send_string("not ready\n\r");

temp_data = FS_ACC_4G;
ret = HAL_I2C_Mem_Write(&hi2c1, (DEV_ADD <<1)+0, REG_CONFIG_ACC, 1, &temp_data, 1, 100);
  if(ret ==HAL_OK)
  {
	  uart_send_string("config acc\n\r");
  }
  else
	  uart_send_string("not ready\n\r");

temp_data = 0x00;
ret = HAL_I2C_Mem_Write(&hi2c1, (DEV_ADD <<1)+0, REG_USR_CTRL, 1, &temp_data, 1, 100);
  if(ret ==HAL_OK)
  {
	  uart_send_string("exit sleep and enable interrupt\n\r");
  }
  else
	  uart_send_string("not ready\n\r");

  MPU_INT_Pin_Init();
}


int16_t read_axis_data(uint8_t high_byte, uint8_t low_byte) {
    return ((int16_t)high_byte << 8) | low_byte;
}

int32_t MPU_read() {
    uint8_t data[2];
    int16_t x_acc;

    // Read 2 bytes of accelerometer data (X-axis only)
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, (DEV_ADD << 1), REG_DATA, 1, data, 2, 100);
    if (status != HAL_OK) {
        uart_send_string("Error: Failed to read accelerometer data\n");
        return -1; // Error reading data
    }

    // Combine high and low bytes for X-axis
    x_acc = ((int16_t)data[0] << 8) | data[1];

    // Debug: Print raw X-axis value
    char buffer[50];
    uart_send_string(buffer);

    // Return the absolute value of X-axis acceleration
    return abs(x_acc);
}

    // Function to detect shakes
uint32_t detect_shakes(uint32_t duration, int threshold) {
        uint32_t start_time = HAL_GetTick();
        uint32_t shake_count = 0;

        while ((HAL_GetTick() - start_time) < duration) {
            int32_t magnitude = MPU_read();

            // Check if magnitude exceeds threshold
            if (magnitude > threshold) {
                shake_count++;
            }

            HAL_Delay(10); // Small delay to avoid flooding
        }

        return shake_count;
    }
