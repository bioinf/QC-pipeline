################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/QcException.cpp \
../src/aho_corasick.cpp \
../src/database.cpp \
../src/fasta_reader.cpp \
../src/main.cpp \
../src/output.cpp \
../src/utils.cpp 

OBJS += \
./src/QcException.o \
./src/aho_corasick.o \
./src/database.o \
./src/fasta_reader.o \
./src/main.o \
./src/output.o \
./src/utils.o 

CPP_DEPS += \
./src/QcException.d \
./src/aho_corasick.d \
./src/database.d \
./src/fasta_reader.d \
./src/main.d \
./src/output.d \
./src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


