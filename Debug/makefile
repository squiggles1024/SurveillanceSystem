################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include NetXDuo/Target/subdir.mk
-include NetXDuo/App/subdir.mk
-include Middlewares/ST/threadx/ports/cortex_m33/gnu/src/subdir.mk
-include Middlewares/ST/threadx/common/src/subdir.mk
-include Middlewares/ST/threadx/common/inc/subdir.mk
-include Middlewares/ST/netxduo/nx_secure/src/subdir.mk
-include Middlewares/ST/netxduo/crypto_libraries/src/subdir.mk
-include Middlewares/ST/netxduo/common/src/subdir.mk
-include Middlewares/ST/netxduo/addons/sntp/subdir.mk
-include Middlewares/ST/netxduo/addons/mqtt/subdir.mk
-include Middlewares/ST/netxduo/addons/dns/subdir.mk
-include Middlewares/ST/netxduo/addons/dhcp/subdir.mk
-include Drivers/STM32U5xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/ExternalHardware/mx_wifi/io_pattern/subdir.mk
-include Core/Src/ExternalHardware/mx_wifi/core/subdir.mk
-include Core/Src/ExternalHardware/mx_wifi/subdir.mk
-include Core/Src/ExternalHardware/subdir.mk
-include Core/Src/BoardSupportPackage/subdir.mk
-include Core/Src/subdir.mk
-include AZURE_RTOS/App/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := SurveillanceSystem
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
SurveillanceSystem.elf \

MAP_FILES += \
SurveillanceSystem.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
SurveillanceSystem.list \


# All Target
all: main-build

# Main-build Target
main-build: SurveillanceSystem.elf secondary-outputs

# Tool invocations
SurveillanceSystem.elf SurveillanceSystem.map: $(OBJS) $(USER_OBJS) C:\Users\evanl\OneDrive\Documents\Embedded_Software_Workspaces\ExampleCode\SurveillanceSystem\STM32U585AIIXQ_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "SurveillanceSystem.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m33 -T"C:\Users\evanl\OneDrive\Documents\Embedded_Software_Workspaces\ExampleCode\SurveillanceSystem\STM32U585AIIXQ_FLASH.ld" --specs=nosys.specs -Wl,-Map="SurveillanceSystem.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

SurveillanceSystem.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "SurveillanceSystem.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) SurveillanceSystem.elf SurveillanceSystem.list SurveillanceSystem.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
