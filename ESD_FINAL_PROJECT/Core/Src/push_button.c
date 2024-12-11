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

void LED_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable Clocks
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Configure Green LEDs (G1 - PD15, G2 - PE15, G3 - PE13)
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_15 | GPIO_PIN_13;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    // Configure Yellow LEDs (Y1 - PE11, Y2 - PE7, Y3 - PA5)
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_7;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure Red LEDs (R1 - PA1, R2 - PC3, R3 - PC1)
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void UpdateLEDs(int health_percentage) {
	    // Reset all LEDs before updating
	    reset_health_leds();

	    if (health_percentage > 75) {
	        // Turn ON all Green LEDs (reversed logic)
	        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET); // G1 - PD15
	        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET); // G2 - PE15
	        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_SET); // G3 - PE13
	    } else if (health_percentage > 40) {
	        // Turn ON all Yellow LEDs (normal logic)
	        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET); // G1 - PD15
	        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET); // G2 - PE15
	        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_RESET); // G3 - PE13
	        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_SET); // Y1 - PE11
	        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);  // Y2 - PE7
	        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);  // Y3 - PA5
	    } else {
	        // Turn ON all Red LEDs (normal logic)
	        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET); // G1 - PD15
	        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET); // G2 - PE15
	        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_RESET); // G3 - PE13
	        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_RESET); // Y1 - PE11
	        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);  // Y2 - PE7
	        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);  // Y3 - PA5
	        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);  // R1 - PA1
	        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);  // R2 - PC3
	        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);  // R3 - PC1
	    }
	}


void reset_health_leds() {
    // Turn off all Green LEDs
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET); // G1 - PD15
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET); // G2 - PE15
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_SET); // G3 - PE13

    // Turn off all Yellow LEDs
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_SET); // Y1 - PE11
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);  // Y2 - PE7
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);  // Y3 - PA5

    // Turn off all Red LEDs
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);  // R1 - PA1
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);  // R2 - PC3
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);  // R3 - PC1
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

