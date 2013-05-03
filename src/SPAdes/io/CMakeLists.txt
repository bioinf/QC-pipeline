project(io CXX)

include_directories(./)
include_directories(../../../include)
include_directories(../../../include/SPAdes)
include_directories(../../../include/SPAdes/io)

add_library(io STATIC
	parser.cpp
	path_helper.cpp)

target_link_libraries(io)
