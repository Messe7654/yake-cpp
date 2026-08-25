#ifndef YAKE_CPP_SRC_DETAIL_TERM_NORMALIZER_H_
#define YAKE_CPP_SRC_DETAIL_TERM_NORMALIZER_H_

#include <string>
#include <string_view>

namespace yake::detail {

[[nodiscard]] std::string normalize_term(std::string_view token);

}  // namespace yake::detail

#endif  // YAKE_CPP_SRC_DETAIL_TERM_NORMALIZER_H_
