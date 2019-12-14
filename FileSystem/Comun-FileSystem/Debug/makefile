################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Serializacion-FileSystem/subdir.mk
-include Manejos-Comunes/subdir.mk
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
all: libComun-FileSystem.so

dependents:
	-cd /home/utnso/workspace/tp-2019-2c-SuperandO/FileSystem/Comun-FileSystem/Release && $(MAKE) clean all
	-cd /home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos/Debug && $(MAKE) clean all

# Tool invocations
libComun-FileSystem.so: $(OBJS) $(USER_OBJS) /home/utnso/workspace/tp-2019-2c-SuperandO/FileSystem/Comun-FileSystem/Release/libComun-FileSystem.so /home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos/Debug/libComunParaTodos.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos/Debug" -shared -o "libComun-FileSystem.so" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(LIBRARIES)$(OBJS)$(C_DEPS) libComun-FileSystem.so
	-@echo ' '

.PHONY: all clean dependents
/home/utnso/workspace/tp-2019-2c-SuperandO/FileSystem/Comun-FileSystem/Release/libComun-FileSystem.so:
/home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos/Debug/libComunParaTodos.so:

-include ../makefile.targets
