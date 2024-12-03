#include "Pressure_Sensor.h"



// Global variables
volatile uint16_t adcValue = 0; // ADC result
volatile uint8_t adcConversionComplete = 0; // Flag to indicate conversion complete

void Pressure_sensor_Init(void) {
    // Enable GPIOB clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // Configure PB1 as analog
    GPIOB->MODER |= (3U << (1 * 2));  // 2 bits per pin, set to 11 (Analog mode)
    GPIOB->PUPDR &= ~(3U << (1 * 2)); // No pull-up/pull-down
}

void ADC_Config(void) {
    // Enable ADC1 clock
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // Set ADC prescaler to PCLK2/4
    ADC->CCR |= (1U << 16); // Bits 17:16 = 01 for PCLK2/4

    // Configure ADC1
    ADC1->CR2 = 0;                     // Reset CR2
    ADC1->CR2 |= ADC_CR2_ADON;         // Enable ADC
    ADC1->CR1 |= ADC_CR1_EOCIE;        // Enable EOC interrupt
    ADC1->CR1 &= ~ADC_CR1_RES;         // 12-bit resolution
    ADC1->SMPR2 |= (3U << (3 * 9));    // Sampling time 56 cycles for channel 9
    ADC1->SQR3 = 9;                    // Set channel 9 (PB1)

    // Enable ADC interrupt in NVIC
    NVIC_EnableIRQ(ADC_IRQn);
    //NVIC_SetPriority(ADC_IRQn, 1);
}

// ADC interrupt handler
void ADC_IRQHandler(void) {
	while (!(ADC1->SR & ADC_SR_EOC));
    //if (ADC1->SR & ADC_SR_EOC) { // Check End of Conversion flag
        adcValue = (uint16_t)ADC1->DR; // Read ADC value
        adcConversionComplete = 1;    // Set conversion complete flag
        ADC1->SR &= ~ADC_SR_EOC;      // Clear EOC flag
}

void ADC_Read(void) {
    // Start ADC conversion
    ADC1->CR2 |= ADC_CR2_SWSTART;
    // Wait for conversion to complete
}

extern void SystemClock_Config(void);
