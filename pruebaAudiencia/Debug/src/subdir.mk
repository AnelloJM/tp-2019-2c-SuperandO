################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/pruebaAudiencia.c 

OBJS += \
./src/pruebaAudiencia.o 

C_DEPS += \
./src/pruebaAudiencia.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2019-2c-SuperandO/SerializacionSuse" -I"/home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos" -I"/home/utnso/workspace/tp-2019-2c-SuperandO/Hilolay" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


