#ifndef YAKE_CPP_SRC_DETAIL_WORD_FEATURES_H_
#define YAKE_CPP_SRC_DETAIL_WORD_FEATURES_H_

#include "detail/stop_words.h"
#include "detail/word_statistics.h"

#include <string>
#include <unordered_map>

namespace yake::detail {

struct WordFeatures {
  double casing              = 0.0;
  double pos                 = 0.0;
  double freq                = 0.0;
  double relatedness         = 0.0;
  double sentence_dispersion = 0.0;
  double score               = 0.0;
};

[[nodiscard]] std::unordered_map<std::string, WordFeatures> calculate_word_features(const DocumentStatistics& doc,
                                                                                    const StopWordSet& stop_words = {});

}  // namespace yake::detail

#endif  // YAKE_CPP_SRC_DETAIL_WORD_FEATURES_H_
