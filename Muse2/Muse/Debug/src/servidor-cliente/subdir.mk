################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/servidor-cliente/conexion_muse.c 

OBJS += \
./src/servidor-cliente/conexion_muse.o 

C_DEPS += \
./src/servidor-cliente/conexion_muse.d 


# Each subdirectory must supply rules for building sources it contributes
src/servidor-cliente/%.o: ../src/servidor-cliente/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/fluor/eclipse-workspace/libmuse" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


