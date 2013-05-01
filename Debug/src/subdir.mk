################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/QcException.cpp \
../src/aho_corasick.cpp \
../src/database.cpp \
../src/job_wrappers.cpp \
../src/main.cpp \
../src/output.cpp \
../src/running_modes.cpp \
../src/ssw_cpp.cpp \
../src/utils.cpp 

C_SRCS += \
../src/ssw.c 

OBJS += \
./src/QcException.o \
./src/aho_corasick.o \
./src/database.o \
./src/job_wrappers.o \
./src/main.o \
./src/output.o \
./src/running_modes.o \
./src/ssw.o \
./src/ssw_cpp.o \
./src/utils.o 

C_DEPS += \
./src/ssw.d 

CPP_DEPS += \
./src/QcException.d \
./src/aho_corasick.d \
./src/database.d \
./src/job_wrappers.d \
./src/main.d \
./src/output.d \
./src/running_modes.d \
./src/ssw_cpp.d \
./src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/opt/local/include/gcc47/c++/ -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/include" -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/include/SPAdes" -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/src/SPAdes" -I/opt/local/include -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/Users/Kos/Dropbox/Bioinf/QC-pipeline/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


