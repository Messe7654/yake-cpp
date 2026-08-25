#ifndef YAKE_CPP_SRC_DETAIL_CANDIDATE_GENERATOR_H_
#define YAKE_CPP_SRC_DETAIL_CANDIDATE_GENERATOR_H_

#include "detail/stop_words.h"
#include "detail/tokenizer.h"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace yake::detail {

struct Candidate {
  std::string text = {};
  std::string norm = {};

  std::vector<std::string> terms = {};

  std::size_t freq               = 0;
  std::size_t first_pos          = 0;
  std::size_t selected_text_freq = 0;

  std::unordered_map<std::string, std::size_t> text_frequencies = {};
};

using CandidateMap = std::unordered_map<std::string, Candidate>;

[[nodiscard]] CandidateMap generate_candidates(const std::vector<Token>& tokens, std::size_t max_ngram_size,
                                               const StopWordSet& stop_words = {});

}  // namespace yake::detail

#endif  // YAKE_CPP_SRC_DETAIL_CANDIDATE_GENERATOR_H_
