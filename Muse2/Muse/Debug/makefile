################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/servidor-cliente/subdir.mk
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
all: Muse

dependents:
	-cd /home/fluor/eclipse-workspace/libmuse/Debug && $(MAKE) clean all

# Tool invocations
Muse: $(OBJS) $(USER_OBJS) /home/fluor/eclipse-workspace/libmuse/Debug/liblibmuse.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/fluor/eclipse-workspace/libmuse/Debug" -o "Muse" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) Muse
	-@echo ' '

.PHONY: all clean dependents
/home/fluor/eclipse-workspace/libmuse/Debug/liblibmuse.so:

-include ../makefile.targets
