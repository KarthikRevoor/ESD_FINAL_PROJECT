#include "uart_init.h"


void pin_init(void)
{
// Enable the USART2 and GPIOA clocks
RCC->APB1ENR |= RCC_APB1ENR_USART2EN;  // Enable UART2 clock
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;   // Enable GPIOA clock

// Configure GPIOA pins PA2 and PA3 for USART2 TX and RX functionality
GPIOA->MODER &= ~((3 << (2 * 2)) | (3 << (3 * 2))); // Clear mode for PA2, PA3
GPIOA->MODER |= (2 << (2 * 2)) | (2 << (3 * 2));    // Set PA2, PA3 to Alternate Function mode
GPIOA->AFR[0] |= (7 << (2 * 4)) | (7 << (3 * 4));   // Set AF7 (UART2) for PA2, PA3

// Configure USART2 for 9600 baud rate, 8 data bits, no parity, 1 stop bit
USART2->BRR = (24000000 / 9600);                    // Set baud rate (assuming 16 MHz clock)
USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;         // Enable USART TX and RX
USART2->CR1 |= USART_CR1_RXNEIE;                    // Enable RX interrupt
USART2->CR1 |= USART_CR1_UE;                        // Enable UART2

// Enable USART2 interrupt in the NVIC
NVIC_EnableIRQ(USART2_IRQn);
}

// USART2 interrupt handler
void USART2_IRQHandler(void) {
    if (USART2->SR & USART_SR_RXNE) {  // RX interrupt triggered
        char received = USART2->DR;    // Read received character

        // Wait until TX is ready, then echo back the received character
        while (!(USART2->SR & USART_SR_TXE));
        USART2->DR = received;         // Transmit received character back
    }
}

void uart_send_string(const char *str) {
    while (*str) {
        while (!(USART2->SR & USART_SR_TXE)); // Wait until TX is ready
        USART2->DR = *str++;                  // Transmit character
    }
}
