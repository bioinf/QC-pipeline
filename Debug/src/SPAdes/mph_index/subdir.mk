################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/SPAdes/mph_index/MurmurHash3.cpp 

CC_SRCS += \
../src/SPAdes/mph_index/mph_bits.cc \
../src/SPAdes/mph_index/mph_index.cc \
../src/SPAdes/mph_index/trigraph.cc 

OBJS += \
./src/SPAdes/mph_index/MurmurHash3.o \
./src/SPAdes/mph_index/mph_bits.o \
./src/SPAdes/mph_index/mph_index.o \
./src/SPAdes/mph_index/trigraph.o 

CC_DEPS += \
./src/SPAdes/mph_index/mph_bits.d \
./src/SPAdes/mph_index/mph_index.d \
./src/SPAdes/mph_index/trigraph.d 

CPP_DEPS += \
./src/SPAdes/mph_index/MurmurHash3.d 


# Each subdirectory must supply rules for building sources it contributes
src/SPAdes/mph_index/%.o: ../src/SPAdes/mph_index/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/opt/local/include/gcc47/c++/ -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/include" -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/include/SPAdes" -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/src/SPAdes" -I/opt/local/include -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/SPAdes/mph_index/%.o: ../src/SPAdes/mph_index/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/opt/local/include/gcc47/c++/ -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/include" -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/include/SPAdes" -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/src/SPAdes" -I/opt/local/include -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


