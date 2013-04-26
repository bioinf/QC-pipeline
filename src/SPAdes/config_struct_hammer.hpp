//***************************************************************************
//* Copyright (c) 2011-2013 Saint-Petersburg Academic University
//* All Rights Reserved
//* See file LICENSE for details.
//****************************************************************************

/*
 * config_struct_hammer.hpp
 *
 *  Created on: Aug 15, 2011
 *      Author: snikolenko
 */

#ifndef CONFIG_STRUCT_HAMMER_HPP_
#define CONFIG_STRUCT_HAMMER_HPP_

#include "standard.hpp"
#include "config_common.hpp"
#include "boost/optional.hpp"

#define CONFIG_FILENAME "/Users/Kos/Dropbox/Bioinf/QC-pipeline/src/SPAdes/config.inp"

// struct for debruijn project's configuration file
struct hammer_config
{
  string input_paired_1;
  string input_paired_2;
  string input_single;
  string input_solid_kmers;
  string input_working_dir;
  int input_trim_quality;
  boost::optional<int> input_qvoffset_opt;
  int input_qvoffset;
  bool input_read_solid_kmers;

  bool general_do_everything_after_first_iteration;
  bool general_reconstruct_only;
  int general_hard_memory_limit;
  unsigned general_max_nthreads;
  int general_tau;
  unsigned general_max_iterations;
  bool general_debug;

  bool count_do;
  unsigned count_numfiles;
  unsigned count_merge_nthreads;
  size_t count_split_buffer;

  bool hamming_do;
  unsigned hamming_blocksize_quadratic_threshold;

  bool bayes_do;
  unsigned bayes_nthreads;
  double bayes_singleton_threshold;
  double bayes_nonsingleton_threshold;
  bool bayes_discard_only_singletons;
  unsigned bayes_debug_output;
  bool bayes_use_hamming_dist;
  bool bayes_hammer_mode;
  bool bayes_write_solid_kmers;
  bool bayes_write_bad_kmers;

  bool expand_do;
  unsigned expand_max_iterations;
  unsigned expand_nthreads;
  bool expand_write_each_iteration;
  bool expand_write_kmers_result;

  bool correct_do;
  bool correct_discard_bad;
  bool correct_use_threshold;
  double correct_threshold;
  unsigned correct_readbuffer;
  unsigned correct_nthreads;
  bool correct_notrim;
};


// main debruijn config load function
void load(hammer_config& cfg, boost::property_tree::ptree const& pt);

typedef config_common::config<hammer_config> cfg;

#endif
