################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Serializacion-FileSystem/Serializacion-FileSystem.c 

OBJS += \
./Serializacion-FileSystem/Serializacion-FileSystem.o 

C_DEPS += \
./Serializacion-FileSystem/Serializacion-FileSystem.d 


# Each subdirectory must supply rules for building sources it contributes
Serializacion-FileSystem/%.o: ../Serializacion-FileSystem/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


