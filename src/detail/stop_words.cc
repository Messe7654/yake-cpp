#include "detail/stop_words.h"
#include "detail/stop_words_data.h"
#include "detail/term_normalizer.h"

#include <stdexcept>
#include <string>
#include <string_view>

namespace yake::detail {
namespace {

StopWordSet parse_stop_words(std::string_view data) {
  StopWordSet stop_words{};

  while (!data.empty()) {
    const std::size_t line_end{data.find('\n')};
    std::string_view word{data.substr(0, line_end)};
    if (!word.empty() && word.back() == '\r') word.remove_suffix(1);
    if (!word.empty()) stop_words.emplace(word);

    if (line_end == std::string_view::npos) break;
    data.remove_prefix(line_end + 1);
  }

  return stop_words;
}

}  // namespace

const StopWordSet& stop_words_for_language(std::string_view language) {
  static const StopWordSet english_stop_words{parse_stop_words(kEnglishStopWordsData)};

  if (language == "en" || language == "english") return english_stop_words;
  throw std::invalid_argument("unsupported language: " + std::string{language});
}

bool is_stop_word(std::string_view token, const StopWordSet& stop_words) {
  if (token.size() < 3 || stop_words.find(std::string{token}) != stop_words.end()) return true;
  const std::string term{normalize_term(token)};
  return stop_words.find(term) != stop_words.end();
}

}  // namespace yake::detail
