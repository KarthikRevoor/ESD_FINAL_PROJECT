################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/DHT11.c \
../Core/Src/DS18b20.c \
../Core/Src/IRsensor.c \
../Core/Src/Integration.c \
../Core/Src/Pressure_Sensor.c \
../Core/Src/fonts.c \
../Core/Src/humidity_sensor.c \
../Core/Src/main.c \
../Core/Src/st7735.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/uart_init.c 

OBJS += \
./Core/Src/DHT11.o \
./Core/Src/DS18b20.o \
./Core/Src/IRsensor.o \
./Core/Src/Integration.o \
./Core/Src/Pressure_Sensor.o \
./Core/Src/fonts.o \
./Core/Src/humidity_sensor.o \
./Core/Src/main.o \
./Core/Src/st7735.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/uart_init.o 

C_DEPS += \
./Core/Src/DHT11.d \
./Core/Src/DS18b20.d \
./Core/Src/IRsensor.d \
./Core/Src/Integration.d \
./Core/Src/Pressure_Sensor.d \
./Core/Src/fonts.d \
./Core/Src/humidity_sensor.d \
./Core/Src/main.d \
./Core/Src/st7735.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/uart_init.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/DHT11.cyclo ./Core/Src/DHT11.d ./Core/Src/DHT11.o ./Core/Src/DHT11.su ./Core/Src/DS18b20.cyclo ./Core/Src/DS18b20.d ./Core/Src/DS18b20.o ./Core/Src/DS18b20.su ./Core/Src/IRsensor.cyclo ./Core/Src/IRsensor.d ./Core/Src/IRsensor.o ./Core/Src/IRsensor.su ./Core/Src/Integration.cyclo ./Core/Src/Integration.d ./Core/Src/Integration.o ./Core/Src/Integration.su ./Core/Src/Pressure_Sensor.cyclo ./Core/Src/Pressure_Sensor.d ./Core/Src/Pressure_Sensor.o ./Core/Src/Pressure_Sensor.su ./Core/Src/fonts.cyclo ./Core/Src/fonts.d ./Core/Src/fonts.o ./Core/Src/fonts.su ./Core/Src/humidity_sensor.cyclo ./Core/Src/humidity_sensor.d ./Core/Src/humidity_sensor.o ./Core/Src/humidity_sensor.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/st7735.cyclo ./Core/Src/st7735.d ./Core/Src/st7735.o ./Core/Src/st7735.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/uart_init.cyclo ./Core/Src/uart_init.d ./Core/Src/uart_init.o ./Core/Src/uart_init.su

.PHONY: clean-Core-2f-Src

