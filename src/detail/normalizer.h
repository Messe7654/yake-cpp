#ifndef YAKE_CPP_SRC_DETAIL_NORMALIZER_H_
#define YAKE_CPP_SRC_DETAIL_NORMALIZER_H_

#include <string>
#include <string_view>

namespace yake::detail {

[[nodiscard]] std::string normalize_token(std::string_view token);

}  // namespace yake::detail

#endif  // YAKE_CPP_SRC_DETAIL_NORMALIZER_H_
