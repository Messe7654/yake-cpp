#ifndef YAKE_CPP_SRC_DETAIL_SIMILARITY_H_
#define YAKE_CPP_SRC_DETAIL_SIMILARITY_H_

#include <string_view>

namespace yake::detail {

[[nodiscard]] double levenshtein_similarity(std::string_view lhs, std::string_view rhs);
[[nodiscard]] double jaro_winkler_similarity(std::string_view lhs, std::string_view rhs);
[[nodiscard]] double sequence_matcher_similarity(std::string_view lhs, std::string_view rhs);

}  // namespace yake::detail

#endif  // YAKE_CPP_SRC_DETAIL_SIMILARITY_H_
