#include "DS18b20.h"

int presence=0, is_received=0;
uint8_t receive_data[8], temp_lsb=0, temp_msb=0;
uint16_t temp;
float temperature;
void temp_uart(uint32_t baud)
{
	  huart1.Instance = USART1;
	  huart1.Init.BaudRate = baud;
	  huart1.Init.WordLength = UART_WORDLENGTH_8B;
	  huart1.Init.StopBits = UART_STOPBITS_1;
	  huart1.Init.Parity = UART_PARITY_NONE;
	  huart1.Init.Mode = UART_MODE_TX_RX;
	  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	  if (HAL_HalfDuplex_Init(&huart1) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

int temp_start(void)
{
	uint8_t data=0xf0;
	temp_uart(9600);
	HAL_UART_Transmit(&huart1,&data,1,100);
	if(HAL_UART_Receive(&huart1,&data,1,1000) != HAL_OK)
		return -1;
	temp_uart(115200);
	if(data==0xf0)
		return -2;
	return 1;
}

int temp_write(uint8_t data)
{
	uint8_t buffer[8];
	for(int i=0;i<8;i++)
	{
		if((data &(1<<i)) !=0)
		{
			buffer[i]=0xFF;
		}
		else
		{
			buffer[i]=0x00;
		}
	}
	HAL_UART_Transmit(&huart1,buffer,8,1000);
}

int temp_read(void)
{
    uint8_t buffer[8];
    uint8_t value = 0;

    for(int i = 0; i < 8; i++)
    {
        buffer[i] = 0xff;
    }

    HAL_UART_Transmit_DMA(&huart1, buffer, 8);
    HAL_UART_Receive_DMA(&huart1, receive_data, 8);

    while (is_received == 0); // Corrected to '=='

    for (int i = 0; i < 8; i++)
    {
        if (receive_data[i] == 0xFF)
        {
            value |= 1 << i;
        }
    }
    is_received = 0; // Reset is_received after read is complete
    return value;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	is_received=1;
}

void temp_sensor_check(void)
{
	presence = temp_start();
	temp_write(0xCC);
	temp_write(0x44);

	presence = temp_start();
	temp_write(0xCC);
	temp_write(0xBE);

	temp_lsb=temp_read();
	temp_msb=temp_read();
	temp=((temp_msb<<8))|temp_lsb;
	temperature = (float)temp/16.0;

	 HAL_Delay(2000);
}
