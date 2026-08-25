#ifndef YAKE_CPP_INCLUDE_YAKE_DETAIL_EXTRACTION_H_
#define YAKE_CPP_INCLUDE_YAKE_DETAIL_EXTRACTION_H_

#include "yake/config.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace yake::detail {

struct RankedKeyword {
  std::string text = {};
  std::string norm = {};

  std::size_t freq      = 0;
  std::size_t first_pos = 0;

  double score = 0.0;
};

void validate_config(const Config& config);
[[nodiscard]] std::vector<RankedKeyword> rank_keywords(const Config& config, std::string_view text);

}  // namespace yake::detail

#endif  // YAKE_CPP_INCLUDE_YAKE_DETAIL_EXTRACTION_H_
