################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: pruebaDeSuse

dependents:
	-cd /home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos/Debug && $(MAKE) clean all
	-cd /home/utnso/workspace/tp-2019-2c-SuperandO/SerializacionSuse/Debug && $(MAKE) clean all

# Tool invocations
pruebaDeSuse: $(OBJS) $(USER_OBJS) /home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos/Debug/libComunParaTodos.so /home/utnso/workspace/tp-2019-2c-SuperandO/SerializacionSuse/Debug/libSerializacionSuse.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos/Debug" -L/home/utnso/workspace/tp-2019-2c-SuperandO/SerializacionSuse/Debug -o "pruebaDeSuse" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) pruebaDeSuse
	-@echo ' '

.PHONY: all clean dependents
/home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos/Debug/libComunParaTodos.so:
/home/utnso/workspace/tp-2019-2c-SuperandO/SerializacionSuse/Debug/libSerializacionSuse.so:

-include ../makefile.targets
