#include "detail/term_normalizer.h"
#include "detail/word_statistics.h"

#include <cstdint>
#include <limits>
#include <string_view>
#include <utf8proc.h>

namespace yake::detail {
namespace {

bool is_uppercase_category(utf8proc_category_t category) {
  return category == UTF8PROC_CATEGORY_LU || category == UTF8PROC_CATEGORY_LT;
}

struct CaseProperties {
  bool capitalized = false;
  bool acronym     = false;
};

CaseProperties inspect_case(std::string_view text) {
  const auto* data{reinterpret_cast<const utf8proc_uint8_t*>(text.data())};
  std::size_t offset{0};
  std::size_t cased_characters{0};
  bool first_is_uppercase{false};
  bool all_cased_are_uppercase{true};

  while (offset < text.size()) {
    utf8proc_int32_t code_point{0};
    const utf8proc_ssize_t len{
        utf8proc_iterate(data + offset, static_cast<utf8proc_ssize_t>(text.size() - offset), &code_point)};
    if (len < 0) break;

    const utf8proc_category_t category{utf8proc_category(code_point)};
    const bool uppercase{is_uppercase_category(category)};
    const bool lowercase{category == UTF8PROC_CATEGORY_LL};
    if (offset == 0) first_is_uppercase = uppercase;
    if (uppercase || lowercase) {
      ++cased_characters;
      if (!uppercase) all_cased_are_uppercase = false;
    }
    offset += static_cast<std::size_t>(len);
  }

  return {first_is_uppercase, cased_characters >= 2 && all_cased_are_uppercase};
}

void collect_context(const std::vector<Token>& tokens, std::size_t idx, std::size_t window_size, bool left,
                     std::unordered_map<std::string, std::size_t>& ctx) {
  std::size_t words_seen{0};
  std::size_t cursor{idx};
  while (words_seen < window_size) {
    if (left) {
      if (cursor == 0) break;
      --cursor;
    }
    else {
      if (cursor == tokens.size() - 1) break;
      ++cursor;
    }

    const Token& neighbor{tokens[cursor]};
    if (neighbor.sentence_idx != tokens[idx].sentence_idx || neighbor.kind == TokenKind::kPunctuation) break;

    ++ctx[normalize_term(neighbor.norm)];
    ++words_seen;
  }
}

}  // namespace

DocumentStatistics collect_word_statistics(const std::vector<Token>& tokens, std::size_t window_size) {
  DocumentStatistics doc{};
  std::size_t last_sentence{std::numeric_limits<std::size_t>::max()};
  bool sentence_has_word{false};

  for (std::size_t idx{0}; idx < tokens.size(); ++idx) {
    const Token& token{tokens[idx]};
    if (token.kind != TokenKind::kWord) continue;

    if (token.sentence_idx != last_sentence) {
      last_sentence     = token.sentence_idx;
      sentence_has_word = false;
      ++doc.sentence_count;
    }

    const std::string term{normalize_term(token.norm)};
    WordStatistics& stats{doc.words[term]};
    if (stats.freq == 0) stats.norm = term;
    ++stats.freq;
    ++doc.word_count;
    if (stats.sentence_pos.empty() || stats.sentence_pos.back() != token.sentence_idx)
      stats.sentence_pos.push_back(token.sentence_idx);

    const CaseProperties case_properties{inspect_case(token.text)};
    if (sentence_has_word && case_properties.capitalized) ++stats.capitalized_freq;
    if (case_properties.acronym) ++stats.acronym_freq;
    sentence_has_word = true;

    collect_context(tokens, idx, window_size, true, stats.left_context);
    collect_context(tokens, idx, window_size, false, stats.right_context);
  }

  return doc;
}

}  // namespace yake::detail
