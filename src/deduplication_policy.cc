#include "detail/similarity.h"
#include "yake/deduplication_policy.h"

namespace yake {

double LevenshteinPolicy::similarity(std::string_view lhs, std::string_view rhs) const {
  return detail::levenshtein_similarity(lhs, rhs);
}

double JaroWinklerPolicy::similarity(std::string_view lhs, std::string_view rhs) const {
  return detail::jaro_winkler_similarity(lhs, rhs);
}

double SequenceMatcherPolicy::similarity(std::string_view lhs, std::string_view rhs) const {
  return detail::sequence_matcher_similarity(lhs, rhs);
}

}  // namespace yake
