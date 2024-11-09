################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/oled_ssd1306/ssd1306.c \
../Drivers/oled_ssd1306/ssd1306_fonts.c \
../Drivers/oled_ssd1306/ssd1306_tests.c 

OBJS += \
./Drivers/oled_ssd1306/ssd1306.o \
./Drivers/oled_ssd1306/ssd1306_fonts.o \
./Drivers/oled_ssd1306/ssd1306_tests.o 

C_DEPS += \
./Drivers/oled_ssd1306/ssd1306.d \
./Drivers/oled_ssd1306/ssd1306_fonts.d \
./Drivers/oled_ssd1306/ssd1306_tests.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/oled_ssd1306/%.o Drivers/oled_ssd1306/%.su Drivers/oled_ssd1306/%.cyclo: ../Drivers/oled_ssd1306/%.c Drivers/oled_ssd1306/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I"C:/Users/gabriel.brunheira/Git/pessoal/project-reggae/proof-of-concepts/oled_ssd1306_test/Drivers/oled_ssd1306" -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-oled_ssd1306

clean-Drivers-2f-oled_ssd1306:
	-$(RM) ./Drivers/oled_ssd1306/ssd1306.cyclo ./Drivers/oled_ssd1306/ssd1306.d ./Drivers/oled_ssd1306/ssd1306.o ./Drivers/oled_ssd1306/ssd1306.su ./Drivers/oled_ssd1306/ssd1306_fonts.cyclo ./Drivers/oled_ssd1306/ssd1306_fonts.d ./Drivers/oled_ssd1306/ssd1306_fonts.o ./Drivers/oled_ssd1306/ssd1306_fonts.su ./Drivers/oled_ssd1306/ssd1306_tests.cyclo ./Drivers/oled_ssd1306/ssd1306_tests.d ./Drivers/oled_ssd1306/ssd1306_tests.o ./Drivers/oled_ssd1306/ssd1306_tests.su

.PHONY: clean-Drivers-2f-oled_ssd1306

