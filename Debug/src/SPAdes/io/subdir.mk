################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/SPAdes/io/parser.cpp \
../src/SPAdes/io/path_helper.cpp 

OBJS += \
./src/SPAdes/io/parser.o \
./src/SPAdes/io/path_helper.o 

CPP_DEPS += \
./src/SPAdes/io/parser.d \
./src/SPAdes/io/path_helper.d 


# Each subdirectory must supply rules for building sources it contributes
src/SPAdes/io/%.o: ../src/SPAdes/io/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/opt/local/include/gcc47/c++/ -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/include" -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/include/SPAdes" -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/src/SPAdes" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


