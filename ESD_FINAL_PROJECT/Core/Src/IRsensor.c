#include "IRSensor.h"
#include "stm32f4xx_hal.h"

// Variable to indicate detection
volatile uint32_t ir_triggered = 0;

// Initialize the IR sensor pin with interrupt
void IR_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable GPIO clock for GPIOA
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Configure PA7 as input with external interrupt
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;  // Interrupt on falling edge
    GPIO_InitStruct.Pull = GPIO_NOPULL;          // No pull-up/pull-down
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Enable the interrupt in the NVIC
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);    // Set priority for EXTI Line 9-5
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);           // Enable interrupt
}

// Callback for handling the interrupt
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_7) { // Check if interrupt is from PA7
        ir_triggered = 1;         // Set flag indicating detection
    }
}

// EXTI IRQ Handler
void EXTI9_5_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7); // Call HAL handler to handle the interrupt
}

// Optional: Function to get the IR trigger state (can be used in main logic)
uint8_t IR_is_triggered(void) {
    if (ir_triggered) {
        ir_triggered = 0; // Reset the flag after reading
        return 1;         // Return true if an object was detected
    }
    return 0;             // No detection
}
