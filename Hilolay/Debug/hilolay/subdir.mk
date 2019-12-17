################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hilolay/hilolay_alumnos.c \
../hilolay/hilolay_internal.c 

OBJS += \
./hilolay/hilolay_alumnos.o \
./hilolay/hilolay_internal.o 

C_DEPS += \
./hilolay/hilolay_alumnos.d \
./hilolay/hilolay_internal.d 


# Each subdirectory must supply rules for building sources it contributes
hilolay/%.o: ../hilolay/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2019-2c-SuperandO/SerializacionSuse" -I"/home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


