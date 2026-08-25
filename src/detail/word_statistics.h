#ifndef YAKE_CPP_SRC_DETAIL_WORD_STATISTICS_H_
#define YAKE_CPP_SRC_DETAIL_WORD_STATISTICS_H_

#include "detail/tokenizer.h"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace yake::detail {

struct WordStatistics {
  std::string norm = {};

  std::size_t freq             = 0;
  std::size_t capitalized_freq = 0;
  std::size_t acronym_freq     = 0;

  std::vector<std::size_t> sentence_pos = {};

  std::unordered_map<std::string, std::size_t> left_context  = {};
  std::unordered_map<std::string, std::size_t> right_context = {};
};

struct DocumentStatistics {
  std::unordered_map<std::string, WordStatistics> words = {};

  std::size_t word_count     = 0;
  std::size_t sentence_count = 0;
};

[[nodiscard]] DocumentStatistics collect_word_statistics(const std::vector<Token>& tokens, std::size_t window_size);

}  // namespace yake::detail

#endif  // YAKE_CPP_SRC_DETAIL_WORD_STATISTICS_H_
