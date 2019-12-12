################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/servidor-cliente/conexion_muse.c \
../src/servidor-cliente/conexion_proceso.c 

OBJS += \
./src/servidor-cliente/conexion_muse.o \
./src/servidor-cliente/conexion_proceso.o 

C_DEPS += \
./src/servidor-cliente/conexion_muse.d \
./src/servidor-cliente/conexion_proceso.d 


# Each subdirectory must supply rules for building sources it contributes
src/servidor-cliente/%.o: ../src/servidor-cliente/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


