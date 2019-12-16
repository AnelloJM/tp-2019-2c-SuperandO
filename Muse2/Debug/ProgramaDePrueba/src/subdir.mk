################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ProgramaDePrueba/src/Prueba.c 

OBJS += \
./ProgramaDePrueba/src/Prueba.o 

C_DEPS += \
./ProgramaDePrueba/src/Prueba.d 


# Each subdirectory must supply rules for building sources it contributes
ProgramaDePrueba/src/%.o: ../ProgramaDePrueba/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Icommons -Ipthread -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


