/*
 * uart_init.c
 *
 *  Created on: Nov 4, 2024
 *      Author: karth
 */

#include "uart_init.h"
// UART initialization
#define SYSTEM_CLOCK 16000000
#define BAUD_RATE 9600
volatile uint8_t tx_complete = 1;
void UART_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;  // Enable USART2 clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;   // Enable GPIOA clock

    // Configure GPIOA PA2 and PA3 for USART2 TX and RX
    GPIOA->MODER &= ~((3 << (2 * 2)) | (3 << (3 * 2)));
    GPIOA->MODER |= (2 << (2 * 2)) | (2 << (3 * 2));
    GPIOA->AFR[0] |= (7 << (2 * 4)) | (7 << (3 * 4));

    USART2->BRR = (SYSTEM_CLOCK / BAUD_RATE);     // Set baud rate (assuming 16 MHz clock)
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;   // Enable TX and RX
    USART2->CR1 |= USART_CR1_RXNEIE;              // Enable RX interrupt
    USART2->CR1 |= USART_CR1_UE;                  // Enable USART2

    NVIC_EnableIRQ(USART2_IRQn);                  // Enable USART2 interrupt in NVIC
}
//void USARTx_IRQHandler(void) {
//    if (USART1->SR & USART_SR_TXE) {  // Check if the TXE flag is set
//        if (!tx_complete) {  // Check if there's data left to send
//            USART1->DR = 'D';  // Example: Sending character 'D'
//            tx_complete = 1;  // Update flag indicating data has been sent
//        } else {
//            // Disable the TXE interrupt to stop sending
//            USART1->CR1 &= ~USART_CR1_TXEIE;
//        }
//    }
//
//    if (USART1->SR & USART_SR_TC) {  // Check if the TC flag is set
//        USART1->SR &= ~USART_SR_TC;  // Clear the TC flag
//    }
//}
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

