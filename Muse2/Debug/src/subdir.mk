################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/libmuse.c \
../src/muse.c 

OBJS += \
./src/libmuse.o \
./src/muse.o 

C_DEPS += \
./src/libmuse.d \
./src/muse.d 


# Each subdirectory must supply rules for building sources it contributes
src/libmuse.o: ../src/libmuse.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Icommons -Ipthread -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/libmuse.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/muse.o: ../src/muse.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Ipthread -Icommons -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/muse.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


