################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/conexion_test.c \
../src/libmuse.c \
../src/muse_init.c 

OBJS += \
./src/conexion_test.o \
./src/libmuse.o \
./src/muse_init.o 

C_DEPS += \
./src/conexion_test.d \
./src/libmuse.d \
./src/muse_init.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


