#include "detail/stop_words.h"
#include "detail/stop_words_data.h"

#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utf8proc.h>

namespace yake::detail {
namespace {

std::size_t code_point_count(std::string_view text) {
  const auto* data{reinterpret_cast<const utf8proc_uint8_t*>(text.data())};
  std::size_t count{0};

  for (std::size_t offset{0}; offset < text.size(); ++count) {
    utf8proc_int32_t code_point{0};
    const utf8proc_ssize_t len{
        utf8proc_iterate(data + offset, static_cast<utf8proc_ssize_t>(text.size() - offset), &code_point)};
    offset += len > 0 ? static_cast<std::size_t>(len) : 1;
  }

  return count;
}

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
  return code_point_count(token) < 3 || stop_words.find(std::string{token}) != stop_words.end();
}

}  // namespace yake::detail
