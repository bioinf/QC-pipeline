add_subdirectory(io)
add_subdirectory(mph_index)

include_directories(./)
include_directories(../../include)
include_directories(../../include/SPAdes)

add_library(SPAdes STATIC
	config_struct_hammer.cpp
	kmer_data.cpp)

target_link_libraries(SPAdes io mph_index)	

