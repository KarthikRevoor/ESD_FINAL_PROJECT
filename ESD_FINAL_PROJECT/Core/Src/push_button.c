#include "push_button.h"
#include "uart_init.h"

volatile uint8_t button_state = 0; // Tracks the button press state

void Push_Button_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable GPIOC clock
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Configure PC13 as input with pull-up resistor
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // Falling edge trigger
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Enable and set EXTI Line 15-10 Interrupt to the lowest priority
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

// Interrupt handler for PC13
void EXTI15_10_IRQHandler(void) {
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13); // Clear interrupt flag

        button_state ^= 1; // Toggle button state

        // Debug: Print button state
        if (button_state) {
            uart_send_string("Button pressed: Battle selected\n\r");
        } else {
            uart_send_string("Button released: Capture selected\n\r");
        }
    }
}

uint8_t Get_Button_State(void) {
    return button_state;
}

// Function to reset button state for repeated selections
void Reset_Button_State(void) {
    button_state = 0; // Reset the button state to default
    uart_send_string("Button state reset. Ready for new selection.\n\r");
}
