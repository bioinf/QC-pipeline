#include <set>
#include "job_wrappers.h"

double is_alignment_good(const StripedSmithWaterman::Alignment& a, const std::string& query, int aligned_part_fraction) {
	return (min(a.query_end, a.ref_end) - max(a.query_begin, a.ref_begin)) / (double) query.size() > aligned_part_fraction;
}

bool AlignmentJobWrapper::operator()(const Read &r) {
	try {
		std::string name = r.getName();
		std::string ref = r.getSequenceString();
		std::map<std::string *, std::string *>::const_iterator it = data->get_data_iterator();
		for (int i = 0; i < data->get_sequences_amount(); ++i) {
			StripedSmithWaterman::Aligner aligner;
			StripedSmithWaterman::Filter filter;
			StripedSmithWaterman::Alignment alignment;
			std::string& query = *(it->second);
			aligner.Align(query.c_str(), ref.c_str(), ref.size(), filter, &alignment);

			std::string database_comment;
			std::string& database_name = *(it->first);
			data->get_comment_by_name(database_name, database_comment);

			if (alignment.mismatches < mismatch_threshold && is_alignment_good(alignment, query, aligned_part_fraction)) {
#pragma omp critical
				print_alignment(output, alignment, ref, query, name, database_name, database_comment);
				print_bed(bed, name, alignment.ref_begin, alignment.ref_end);
			}
			it++;
		}
	} catch (std::exception& e) {
		ERROR(e.what());
	}
	return false;
}

bool ExactMatchJobWrapper::operator()(const Read &r) {
	try{
		std::string name = r.getName();
		std::string sequence = r.getSequenceString();
		ahoCorasick.search(sequence);
		std::map<std::string*, std::vector<int>, Compare> res = ahoCorasick.getMatch();

		if (res.size() > 0) {
#pragma omp critical
			print_match(output, bed, res, name, sequence, data);
		}
	} catch (std::exception& e) {
		ERROR(e.what());
	}
	return false;
}

bool ExactAndAlignJobWrapper::operator()(const Read &r) {
	try{
		std::string name = r.getName();
		std::string sequence = r.getSequenceString();
		ahoCorasick.search(sequence);
		std::map<std::string*, std::vector<int>, Compare> matchingKmers = ahoCorasick.getMatch();

		std::set<std::string * , Compare> setOfContaminations2check;
		for (std::map<std::string*, std::vector<int>, Compare>::const_iterator it = matchingKmers.begin(); it != matchingKmers.end(); ++it) {
			std::vector<std::string *> listOfseqs;
			data->get_sequences_for_kmer(*(it->first), listOfseqs);
			setOfContaminations2check.insert(listOfseqs.begin(), listOfseqs.end());
		}

		//try to exact match the sequences
		AhoCorasick ac;
		for (std::set<std::string * , Compare>::const_iterator it = setOfContaminations2check.begin(); it != setOfContaminations2check.end(); ++it) {
			ac.addString(*it);
		}

		std::map<std::string*, std::vector<int>, Compare> matchingSequences = ac.getMatch();
		if (!matchingSequences.empty()) {
#pragma omp critical
			print_match(output, bed, matchingSequences, name, sequence, data);
		}
		ac.cleanup();

		//try to align the sequences
		for (std::set<std::string * , Compare>::const_iterator it = setOfContaminations2check.begin(); it != setOfContaminations2check.end(); ++it) {
			StripedSmithWaterman::Aligner aligner;
			StripedSmithWaterman::Filter filter;
			StripedSmithWaterman::Alignment alignment;
			std::string& query = *(*it);
			aligner.Align(query.c_str(), sequence.c_str(), sequence.size(), filter, &alignment);

			std::string database_comment;
			std::string database_name;
			data->get_name_by_sequence(query, database_name);
			data->get_comment_by_name(database_name, database_comment);

#pragma omp critical
			if (alignment.mismatches < mismatch_threshold && is_alignment_good(alignment, query, aligned_part_fraction)) {
				print_alignment(output, alignment, sequence, query, name, database_name, database_comment);
				print_bed(bed, name, alignment.ref_begin, alignment.ref_end);
			}
		}

	} catch (std::exception& e) {
		ERROR(e.what());
	}
	return false;
}
