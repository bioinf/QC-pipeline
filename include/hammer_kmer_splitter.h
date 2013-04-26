#ifndef HAMMER_KMER_SPLITTER_H_
#define HAMMER_KMER_SPLITTER_H_

#include "kmer_data.hpp"
#include "io/read_processor.hpp"
#include "valid_kmer_generator.hpp"

#include "io/mmapped_writer.hpp"
#include "io/ireadstream.hpp"
#include "config_struct_hammer.hpp"
using namespace hammer;

class BufferFiller;

class HammerKMerSplitter : public KMerSplitter<hammer::KMer> {
  typedef std::vector<std::vector<KMer> > KMerBuffer;

  void DumpBuffers(size_t num_files, size_t nthreads,
                   std::vector<KMerBuffer> &buffers,
                   MMappedRecordWriter<KMer> *ostreams) const;

 public:
  HammerKMerSplitter(std::string &work_dir)
      : KMerSplitter<hammer::KMer>(work_dir, hammer::K) {}

  virtual path::files_t Split(size_t num_files);

  friend class BufferFiller;
};

#endif /* HAMMER_KMER_SPLITTER_H_ */
