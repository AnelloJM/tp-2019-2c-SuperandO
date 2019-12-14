################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include hilolay/subdir.mk
-include Serializacion/subdir.mk
-include Lista/subdir.mk
-include Conexiones/subdir.mk
-include Archivos/subdir.mk
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
all: libComunParaTodos.so

dependents:
	-cd /home/utnso/workspace/tp-2019-2c-SuperandO/SerializacionSuse/Debug && $(MAKE) clean all

# Tool invocations
libComunParaTodos.so: $(OBJS) $(USER_OBJS) /home/utnso/workspace/tp-2019-2c-SuperandO/SerializacionSuse/Debug/libSerializacionSuse.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/workspace/tp-2019-2c-SuperandO/SerializacionSuse/Debug" -shared -o "libComunParaTodos.so" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(LIBRARIES)$(OBJS)$(C_DEPS) libComunParaTodos.so
	-@echo ' '

.PHONY: all clean dependents
/home/utnso/workspace/tp-2019-2c-SuperandO/SerializacionSuse/Debug/libSerializacionSuse.so:

-include ../makefile.targets
