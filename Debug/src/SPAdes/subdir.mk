################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/SPAdes/config_struct_hammer.cpp \
../src/SPAdes/kmer_data.cpp 

OBJS += \
./src/SPAdes/config_struct_hammer.o \
./src/SPAdes/kmer_data.o 

CPP_DEPS += \
./src/SPAdes/config_struct_hammer.d \
./src/SPAdes/kmer_data.d 


# Each subdirectory must supply rules for building sources it contributes
src/SPAdes/%.o: ../src/SPAdes/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/opt/local/include/gcc47/c++/ -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/include" -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/include/SPAdes" -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/src/SPAdes" -I/opt/local/include -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


