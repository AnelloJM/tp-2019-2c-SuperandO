################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hilolay/hilolay_internal.c 

OBJS += \
./hilolay/hilolay_internal.o 

C_DEPS += \
./hilolay/hilolay_internal.d 


# Each subdirectory must supply rules for building sources it contributes
hilolay/%.o: ../hilolay/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


