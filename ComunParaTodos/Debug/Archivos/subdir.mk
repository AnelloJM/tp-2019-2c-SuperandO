################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Archivos/archivos.c 

OBJS += \
./Archivos/archivos.o 

C_DEPS += \
./Archivos/archivos.d 


# Each subdirectory must supply rules for building sources it contributes
Archivos/%.o: ../Archivos/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2019-2c-SuperandO/SerializacionSuse" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


