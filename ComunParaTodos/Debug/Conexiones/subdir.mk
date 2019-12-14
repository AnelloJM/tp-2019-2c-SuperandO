################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Conexiones/Conexiones.c \
../Conexiones/servidor.c \
../Conexiones/utils.c 

OBJS += \
./Conexiones/Conexiones.o \
./Conexiones/servidor.o \
./Conexiones/utils.o 

C_DEPS += \
./Conexiones/Conexiones.d \
./Conexiones/servidor.d \
./Conexiones/utils.d 


# Each subdirectory must supply rules for building sources it contributes
Conexiones/%.o: ../Conexiones/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2019-2c-SuperandO/SerializacionSuse" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


