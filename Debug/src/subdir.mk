################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/QcException.cpp \
../src/aho_corasick.cpp \
../src/database.cpp \
../src/edit_distance.cpp \
../src/local_alignment.cpp \
../src/main.cpp \
../src/output.cpp \
../src/running_modes.cpp \
../src/utils.cpp 

OBJS += \
./src/QcException.o \
./src/aho_corasick.o \
./src/database.o \
./src/edit_distance.o \
./src/local_alignment.o \
./src/main.o \
./src/output.o \
./src/running_modes.o \
./src/utils.o 

CPP_DEPS += \
./src/QcException.d \
./src/aho_corasick.d \
./src/database.d \
./src/edit_distance.d \
./src/local_alignment.d \
./src/main.d \
./src/output.d \
./src/running_modes.d \
./src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/opt/local/include/gcc47/c++/ -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/include" -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/include/SPAdes" -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/src/SPAdes" -I/opt/local/include -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


