############################################################################
# Copyright (c) 2011-2013 Saint-Petersburg Academic University
# All Rights Reserved
# See file LICENSE for details.
############################################################################

project(mph_index CXX)

include_directories(./)
include_directories(../../../include)
include_directories(../../../include/SPAdes)
include_directories(../../../include/SPAdes/mph_index)

add_library(mph_index STATIC
            MurmurHash3.cpp
            mph_bits.cc
            mph_index.cc
            trigraph.cc)
