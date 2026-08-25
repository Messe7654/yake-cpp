#include "detail/word_features.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <unordered_map>
#include <vector>

namespace yake::detail {
namespace {

double context_probability(const std::unordered_map<std::string, std::size_t>& ctx) {
  const std::size_t total{std::accumulate(ctx.begin(), ctx.end(), std::size_t{0},
                                          [](std::size_t sum, const auto& entry) { return sum + entry.second; })};
  if (total == 0) return 0.0;
  return static_cast<double>(ctx.size()) / total;
}

double median(const std::vector<std::size_t>& values) {
  if (values.empty()) return 0.0;

  const std::size_t middle{values.size() >> 1};
  if (values.size() & 1U) return static_cast<double>(values[middle]);
  return (static_cast<double>(values[middle - 1]) + static_cast<double>(values[middle])) / 2;
}

}  // namespace

std::unordered_map<std::string, WordFeatures> calculate_word_features(const DocumentStatistics& doc,
                                                                      const StopWordSet& stop_words) {
  std::unordered_map<std::string, WordFeatures> result{};
  if (doc.words.empty() || doc.sentence_count == 0) return result;

  std::size_t maximum_freq{0};
  double freq_sum{0.0};
  std::size_t freq_count{0};
  for (const auto& [word, stats] : doc.words) {
    maximum_freq = std::max(maximum_freq, stats.freq);
    if (is_stop_word(word, stop_words)) continue;
    freq_sum += stats.freq;
    ++freq_count;
  }

  const double mean_freq{(freq_count == 0) ? 0.0 : freq_sum / freq_count};
  double squared_difference_sum{0.0};
  for (const auto& [word, stats] : doc.words) {
    if (is_stop_word(word, stop_words)) continue;
    const double difference{stats.freq - mean_freq};
    squared_difference_sum += difference * difference;
  }
  const double freq_deviation{(freq_count == 0) ? 0.0 : std::sqrt(squared_difference_sum / freq_count)};
  const double freq_scale{mean_freq + freq_deviation};

  for (const auto& [word, stats] : doc.words) {
    WordFeatures features{};
    const double term_freq{static_cast<double>(stats.freq)};
    const double norm_freq{term_freq / maximum_freq};
    const double left_prob{context_probability(stats.left_context)};
    const double right_prob{context_probability(stats.right_context)};

    features.relatedness         = (0.5 + left_prob * norm_freq) + (0.5 + right_prob * norm_freq);
    features.freq                = (freq_scale == 0.0) ? 0.0 : term_freq / freq_scale;
    features.sentence_dispersion = static_cast<double>(stats.sentence_pos.size()) / doc.sentence_count;
    features.casing              = std::max(stats.acronym_freq, stats.capitalized_freq) / (1.0 + std::log(term_freq));
    features.pos                 = std::log(std::log(3.0 + median(stats.sentence_pos)));
    features.score = (features.pos * features.relatedness) / (features.casing + features.freq / features.relatedness +
                                                              features.sentence_dispersion / features.relatedness);

    result.emplace(word, features);
  }

  return result;
}

}  // namespace yake::detail
