#ifndef YAKE_CPP_SRC_DETAIL_CANDIDATE_RANKER_H_
#define YAKE_CPP_SRC_DETAIL_CANDIDATE_RANKER_H_

#include "detail/candidate_generator.h"
#include "detail/stop_words.h"
#include "detail/word_features.h"
#include "detail/word_statistics.h"
#include "yake/detail/extraction.h"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace yake::detail {

[[nodiscard]] std::vector<RankedKeyword> rank_candidates(
    const CandidateMap& candidates, const std::unordered_map<std::string, WordFeatures>& word_features,
    const DocumentStatistics& doc = {}, const StopWordSet& stop_words = {});

}  // namespace yake::detail

#endif  // YAKE_CPP_SRC_DETAIL_CANDIDATE_RANKER_H_
