#ifndef YAKE_CPP_SRC_DETAIL_STOP_WORDS_H_
#define YAKE_CPP_SRC_DETAIL_STOP_WORDS_H_

#include <string>
#include <string_view>
#include <unordered_set>

namespace yake::detail {

using StopWordSet = std::unordered_set<std::string>;

[[nodiscard]] const StopWordSet& stop_words_for_language(std::string_view language);
[[nodiscard]] bool is_stop_word(std::string_view token, const StopWordSet& stop_words);

}  // namespace yake::detail

#endif  // YAKE_CPP_SRC_DETAIL_STOP_WORDS_H_
