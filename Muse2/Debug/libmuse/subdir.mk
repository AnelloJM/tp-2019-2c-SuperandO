################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libmuse/conexion_proceso.c \
../libmuse/libmuse.c 

OBJS += \
./libmuse/conexion_proceso.o \
./libmuse/libmuse.o 

C_DEPS += \
./libmuse/conexion_proceso.d \
./libmuse/libmuse.d 


# Each subdirectory must supply rules for building sources it contributes
libmuse/%.o: ../libmuse/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Icommons -Ipthread -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


