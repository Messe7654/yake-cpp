#ifndef YAKE_CPP_INCLUDE_YAKE_DEDUPLICATION_POLICY_H_
#define YAKE_CPP_INCLUDE_YAKE_DEDUPLICATION_POLICY_H_

#include "yake/export.h"

#include <string_view>

namespace yake {

struct YAKE_EXPORT LevenshteinPolicy {
  [[nodiscard]] double similarity(std::string_view lhs, std::string_view rhs) const;
};

struct YAKE_EXPORT JaroWinklerPolicy {
  [[nodiscard]] double similarity(std::string_view lhs, std::string_view rhs) const;
};

struct YAKE_EXPORT SequenceMatcherPolicy {
  [[nodiscard]] double similarity(std::string_view lhs, std::string_view rhs) const;
};

}  // namespace yake

#endif  // YAKE_CPP_INCLUDE_YAKE_DEDUPLICATION_POLICY_H_
