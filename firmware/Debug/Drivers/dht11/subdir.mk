################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/dht11/mk_dht11.c 

OBJS += \
./Drivers/dht11/mk_dht11.o 

C_DEPS += \
./Drivers/dht11/mk_dht11.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/dht11/%.o Drivers/dht11/%.su Drivers/dht11/%.cyclo: ../Drivers/dht11/%.c Drivers/dht11/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Drivers/dht11 -I../Drivers/oled_ssd1306 -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-dht11

clean-Drivers-2f-dht11:
	-$(RM) ./Drivers/dht11/mk_dht11.cyclo ./Drivers/dht11/mk_dht11.d ./Drivers/dht11/mk_dht11.o ./Drivers/dht11/mk_dht11.su

.PHONY: clean-Drivers-2f-dht11

