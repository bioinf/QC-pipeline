cmake_minimum_required(VERSION 2.6)

project(QC-pipeline)
set(CMAKE_CXX_FLAGS "-std=c++11 -fopenmp")
set(CMAKE_CURRENT_BINARY_DIR "../bin")
set(CMAKE_INSTALL_PREFIX "./")
add_subdirectory(SPAdes)

find_package(Boost REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
include_directories(${ZLIB_INCLUDE_DIR})
include_directories(../include)
include_directories(../include/SPAdes)
include_directories(../include/SPAdes/adt)
include_directories(../include/SPAdes/io)
include_directories(../include/SPAdes/sequence)
include_directories(../include/SPAdes/mph_index)
include_directories(../include/SPAdes/logger)
include_directories(SPAdes)
include_directories(SPAdes/io)
include_directories(SPAdes/mph_index)

link_directories(${CMAKE_SOURCE_DIR}/SPAdes)

add_executable(QC-pipeline 
	aho_corasick.cpp
	job_wrappers.cpp
	QcException.cpp
	ssw.c
	database.cpp
	main.cpp
	running_modes.cpp
	ssw_cpp.cpp
	output.cpp
	utils.cpp)

target_link_libraries(QC-pipeline SPAdes ${Boost_LIBRARIES} ${ZLIB_LIBRARIES})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.inp ${CMAKE_CURRENT_BINARY_DIR}/config.inp COPYONLY)
install(TARGETS QC-pipeline DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
