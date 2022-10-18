################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Sensors/HTS221.c \
../Core/Src/Sensors/IIS2MDC.c \
../Core/Src/Sensors/ISM330DHCX.c \
../Core/Src/Sensors/LPS22HH.c \
../Core/Src/Sensors/OV5640.c 

OBJS += \
./Core/Src/Sensors/HTS221.o \
./Core/Src/Sensors/IIS2MDC.o \
./Core/Src/Sensors/ISM330DHCX.o \
./Core/Src/Sensors/LPS22HH.o \
./Core/Src/Sensors/OV5640.o 

C_DEPS += \
./Core/Src/Sensors/HTS221.d \
./Core/Src/Sensors/IIS2MDC.d \
./Core/Src/Sensors/ISM330DHCX.d \
./Core/Src/Sensors/LPS22HH.d \
./Core/Src/Sensors/OV5640.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Sensors/%.o Core/Src/Sensors/%.su: ../Core/Src/Sensors/%.c Core/Src/Sensors/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U585xx -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_NON_SECURE=1 -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I../AZURE_RTOS/App -I../Middlewares/ST/threadx/common/inc -I../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Sensors

clean-Core-2f-Src-2f-Sensors:
	-$(RM) ./Core/Src/Sensors/HTS221.d ./Core/Src/Sensors/HTS221.o ./Core/Src/Sensors/HTS221.su ./Core/Src/Sensors/IIS2MDC.d ./Core/Src/Sensors/IIS2MDC.o ./Core/Src/Sensors/IIS2MDC.su ./Core/Src/Sensors/ISM330DHCX.d ./Core/Src/Sensors/ISM330DHCX.o ./Core/Src/Sensors/ISM330DHCX.su ./Core/Src/Sensors/LPS22HH.d ./Core/Src/Sensors/LPS22HH.o ./Core/Src/Sensors/LPS22HH.su ./Core/Src/Sensors/OV5640.d ./Core/Src/Sensors/OV5640.o ./Core/Src/Sensors/OV5640.su

.PHONY: clean-Core-2f-Src-2f-Sensors

