#ifndef YAKE_CPP_INCLUDE_YAKE_KEYWORD_EXTRACTOR_H_
#define YAKE_CPP_INCLUDE_YAKE_KEYWORD_EXTRACTOR_H_

#include "yake/config.h"
#include "yake/deduplication_policy.h"
#include "yake/detail/extraction.h"
#include "yake/keyword.h"

#include <algorithm>
#include <string_view>
#include <utility>
#include <vector>

namespace yake {

template <typename DedupPolicy = SequenceMatcherPolicy>
class BasicKeywordExtractor {
 public:
  explicit BasicKeywordExtractor(Config config = {}, DedupPolicy policy = {})
      : config_{std::move(config)}, policy_{std::move(policy)} {
    detail::validate_config(config_);
  }

  [[nodiscard]] const Config& config() const noexcept { return config_; }
  [[nodiscard]] const DedupPolicy& policy() const noexcept { return policy_; }

  [[nodiscard]] std::vector<Keyword> extract(std::string_view text) const {
    if (text.empty() || config_.max_keywords == 0) return {};

    const auto ranked{detail::rank_keywords(config_, text)};
    std::vector<Keyword> keywords{};
    std::vector<std::string_view> selected_norms{};
    keywords.reserve(std::min(config_.max_keywords, ranked.size()));
    selected_norms.reserve(std::min(config_.max_keywords, ranked.size()));

    for (const detail::RankedKeyword& cand : ranked) {
      bool duplicate{false};
      if (config_.deduplication_threshold < 1.0) {
        for (const std::string_view norm : selected_norms) {
          if (policy_.similarity(cand.norm, norm) > config_.deduplication_threshold) {
            duplicate = true;
            break;
          }
        }
      }

      if (!duplicate) {
        keywords.push_back({cand.text, cand.score});
        selected_norms.push_back(cand.norm);
      }
      if (keywords.size() == config_.max_keywords) break;
    }
    return keywords;
  }

 private:
  Config config_      = {};
  DedupPolicy policy_ = {};
};

using KeywordExtractor = BasicKeywordExtractor<>;

}  // namespace yake

#endif  // YAKE_CPP_INCLUDE_YAKE_KEYWORD_EXTRACTOR_H_
