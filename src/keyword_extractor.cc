#include "detail/candidate_generator.h"
#include "detail/candidate_ranker.h"
#include "detail/stop_words.h"
#include "detail/tokenizer.h"
#include "detail/word_features.h"
#include "detail/word_statistics.h"
#include "yake/detail/extraction.h"

#include <stdexcept>
#include <utility>
#include <vector>

namespace yake::detail {

void validate_config(const Config& config) {
  if (config.language.empty()) throw std::invalid_argument("language must not be empty");
  static_cast<void>(stop_words_for_language(config.language));
  if (config.max_ngram_size == 0) throw std::invalid_argument("max_ngram_size must be greater than zero");
  if (config.deduplication_threshold < 0.0 || config.deduplication_threshold > 1.0)
    throw std::invalid_argument("deduplication_threshold must be between zero and one");
  if (config.context_window_size == 0) throw std::invalid_argument("context_window_size must be greater than zero");
}

std::vector<RankedKeyword> rank_keywords(const Config& config, std::string_view text) {
  const auto tokens{detail::tokenize(text)};
  const auto doc{detail::collect_word_statistics(tokens, config.context_window_size)};
  const auto& stop_words{detail::stop_words_for_language(config.language)};
  const auto word_features{detail::calculate_word_features(doc, stop_words)};
  const auto candidates{detail::generate_candidates(tokens, config.max_ngram_size, stop_words)};
  return detail::rank_candidates(candidates, word_features, doc, stop_words);
}

}  // namespace yake::detail
