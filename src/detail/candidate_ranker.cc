#include "detail/candidate_ranker.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace yake::detail {
namespace {

double transition_probability(const DocumentStatistics& doc, std::string_view lhs, std::string_view rhs,
                              std::string_view denominator) {
  const auto lhs_it{doc.words.find(std::string{lhs})};
  const auto denominator_it{doc.words.find(std::string{denominator})};
  if (lhs_it == doc.words.end() || denominator_it == doc.words.end() || denominator_it->second.freq == 0) return 0.0;

  const auto ctx_it{lhs_it->second.right_context.find(std::string{rhs})};
  if (ctx_it == lhs_it->second.right_context.end()) return 0.0;
  return static_cast<double>(ctx_it->second) / denominator_it->second.freq;
}

}  // namespace

std::vector<RankedKeyword> rank_candidates(const CandidateMap& candidates,
                                           const std::unordered_map<std::string, WordFeatures>& word_features,
                                           const DocumentStatistics& doc, const StopWordSet& stop_words) {
  std::vector<RankedKeyword> ranked{};
  ranked.reserve(candidates.size());

  for (const auto& [norm, cand] : candidates) {
    double score_product{1.0};
    double score_sum{0.0};
    bool complete{true};

    for (std::size_t idx{0}; idx < cand.terms.size(); ++idx) {
      const std::string& term{cand.terms[idx]};
      const auto it{word_features.find(term)};
      if (it == word_features.end()) {
        complete = false;
        break;
      }
      if (is_stop_word(term, stop_words) && idx > 0 && idx + 1 < cand.terms.size()) {
        const std::string& prev{cand.terms[idx - 1]};
        const std::string& next{cand.terms[idx + 1]};
        const double prob{transition_probability(doc, prev, term, prev) *
                          transition_probability(doc, term, next, next)};
        const double adjustment{1.0 - prob};
        score_product *= 1.0 + adjustment;
        score_sum -= adjustment;
      }
      else {
        score_product *= it->second.score;
        score_sum += it->second.score;
      }
    }

    if (!complete || cand.freq == 0) continue;
    const double score{score_product / ((score_sum + 1.0) * cand.freq)};
    ranked.push_back({cand.text, norm, cand.freq, cand.first_pos, score});
  }

  std::sort(ranked.begin(), ranked.end(), [](const auto& lhs, const auto& rhs) {
    if (lhs.score != rhs.score) return lhs.score < rhs.score;
    if (lhs.first_pos != rhs.first_pos) return lhs.first_pos < rhs.first_pos;
    return lhs.norm < rhs.norm;
  });
  return ranked;
}

}  // namespace yake::detail
