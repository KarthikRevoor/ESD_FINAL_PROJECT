#include "stm32f4xx_hal.h"
#include "push_button.h"
#include "uart_init.h"

// Variables to track button states
volatile uint8_t battle_button_state = 0; // For PC13 (Battle)
volatile uint8_t capture_button_state = 0; // For PC14 (Capture)

// Initialize both buttons: PC13 for Battle, PC14 for Capture
void Push_Button_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable GPIOC clock
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Configure PC13 as input with pull-up resistor (Battle Button)
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // Falling edge trigger
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Configure PC14 as input with pull-up resistor (Capture Button)
    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // Falling edge trigger
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Enable and set EXTI Line 15-10 Interrupt to the lowest priority
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

// Interrupt handler for EXTI lines 13 to 15
void EXTI15_10_IRQHandler(void) {
    // Check for PC13 (Battle Button)
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13); // Clear interrupt flag
        battle_button_state = 1; // Set battle button state
        uart_send_string("Battle button pressed.\n\r");
    }

    // Check for PC14 (Capture Button)
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_14) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_14); // Clear interrupt flag
        capture_button_state = 1; // Set capture button state
        uart_send_string("Capture button pressed.\n\r");
    }
}

// Getter functions for button states
uint8_t Get_Battle_Button_State(void) {
    return battle_button_state;
}

uint8_t Get_Capture_Button_State(void) {
    return capture_button_state;
}

// Reset both button states
void Reset_Button_State(void) {
    battle_button_state = 0;
    capture_button_state = 0;
}
