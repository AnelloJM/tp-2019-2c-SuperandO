################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Serializacion/serializacion.c 

OBJS += \
./Serializacion/serializacion.o 

C_DEPS += \
./Serializacion/serializacion.d 


# Each subdirectory must supply rules for building sources it contributes
Serializacion/%.o: ../Serializacion/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


