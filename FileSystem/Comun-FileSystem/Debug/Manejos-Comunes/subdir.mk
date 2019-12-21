################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Manejos-Comunes/Manejos-Comunes.c 

OBJS += \
./Manejos-Comunes/Manejos-Comunes.o 

C_DEPS += \
./Manejos-Comunes/Manejos-Comunes.d 


# Each subdirectory must supply rules for building sources it contributes
Manejos-Comunes/%.o: ../Manejos-Comunes/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DFUSE_USE_VERSION=27 -D_FILE_OFFSET_BITS=64 -I"/home/utnso/workspace/tp-2019-2c-SuperandO/ComunParaTodos" -I"/home/utnso/workspace/tp-2019-2c-SuperandO/FileSystem/Comun-FileSystem" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


