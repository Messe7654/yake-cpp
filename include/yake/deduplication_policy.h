#ifndef YAKE_CPP_INCLUDE_YAKE_DEDUPLICATION_POLICY_H_
#define YAKE_CPP_INCLUDE_YAKE_DEDUPLICATION_POLICY_H_

#include <string_view>

namespace yake {

struct LevenshteinPolicy {
  [[nodiscard]] double similarity(std::string_view lhs, std::string_view rhs) const;
};

struct JaroWinklerPolicy {
  [[nodiscard]] double similarity(std::string_view lhs, std::string_view rhs) const;
};

struct SequenceMatcherPolicy {
  [[nodiscard]] double similarity(std::string_view lhs, std::string_view rhs) const;
};

}  // namespace yake

#endif  // YAKE_CPP_INCLUDE_YAKE_DEDUPLICATION_POLICY_H_
