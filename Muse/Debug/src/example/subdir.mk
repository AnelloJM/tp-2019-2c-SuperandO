################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/example/cliente.c \
../src/example/prueba_init.c 

OBJS += \
./src/example/cliente.o \
./src/example/prueba_init.o 

C_DEPS += \
./src/example/cliente.d \
./src/example/prueba_init.d 


# Each subdirectory must supply rules for building sources it contributes
src/example/%.o: ../src/example/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


