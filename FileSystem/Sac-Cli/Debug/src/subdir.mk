################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Sac-Cli.c 

OBJS += \
./src/Sac-Cli.o 

C_DEPS += \
./src/Sac-Cli.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFUSE_USE_VERSION=27 -D_FILE_OFFSET_BITS=64 -I"/home/utnso/workspace/tp-2019-2c-SuperandO/FileSystem/Comun-FileSystem" -I"/home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


