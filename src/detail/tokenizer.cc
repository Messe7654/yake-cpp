#include "detail/normalizer.h"
#include "detail/tokenizer.h"

#include <cstdint>
#include <string>

namespace yake::detail {
namespace {

struct CodePoint {
  std::uint32_t value;
  std::size_t len;
};

CodePoint decode_utf8(std::string_view text, std::size_t offset) {
  const auto first{static_cast<unsigned char>(text[offset])};
  if (first < 0x80) return {first, 1};

  std::size_t len{1};
  std::uint32_t value{first};
  if ((first & 0xE0) == 0xC0) {  // 2-byte UTF-8 sequence
    len   = 2;
    value = first & 0x1F;
  }
  else if ((first & 0xF0) == 0xE0) {  // 3-byte UTF-8 sequence
    len   = 3;
    value = first & 0x0F;
  }
  else if ((first & 0xF8) == 0xF0) {  // 4-byte UTF-8 sequence
    len   = 4;
    value = first & 0x07;
  }
  else {  // invalid UTF-8 sequence
    return {first, 1};
  }

  if (offset + len > text.size()) return {first, 1};

  for (std::size_t idx{1}; idx < len; ++idx) {
    const auto continuation{static_cast<unsigned char>(text[offset + idx])};
    if ((continuation & 0xC0) != 0x80) return {first, 1};

    value = (value << 6) | (continuation & 0x3F);
  }
  return {value, len};
}

bool is_space(std::uint32_t value) {
  return value == ' ' || (value >= '\t' && value <= '\r') || value == 0x00A0 || value == 0x1680 ||
         (value >= 0x2000 && value <= 0x200A) || value == 0x2028 || value == 0x2029 || value == 0x202F ||
         value == 0x205F || value == 0x3000;
}

bool is_sentence_end(std::uint32_t value) {
  return value == '.' || value == '!' || value == '?' || value == 0x3002 || value == 0xFF01 || value == 0xFF0E ||
         value == 0xFF1F;
}

bool is_punctuation(std::uint32_t value) {
  if (value < 0x80)
    return (value >= 0x21 && value <= 0x2F) || (value >= 0x3A && value <= 0x40) || (value >= 0x5B && value <= 0x60) ||
           (value >= 0x7B && value <= 0x7E);
  return (value >= 0x2000 && value <= 0x206F) || (value >= 0x3001 && value <= 0x303F) ||
         (value >= 0xFF01 && value <= 0xFF65);
}

bool is_word_connector(std::uint32_t value) {
  return value == '\'' || value == '-' || value == 0x2018 || value == 0x2019 || value == 0x2010 || value == 0x2011;
}

bool has_word_character_at(std::string_view text, std::size_t offset) {
  if (offset >= text.size()) return false;
  const CodePoint code_point{decode_utf8(text, offset)};
  return !is_space(code_point.value) && !is_punctuation(code_point.value);
}

}  // namespace

std::vector<Token> tokenize(std::string_view text) {
  std::vector<Token> tokens{};
  std::string word{};
  std::size_t sentence_idx{0};
  std::size_t pos{0};
  bool pending_sentence{false};

  const auto flush_word{[&]() {
    if (word.empty()) return;

    if (pending_sentence) {
      ++sentence_idx;
      pos              = 0;
      pending_sentence = false;
    }
    tokens.push_back({word, normalize_token(word), TokenKind::kWord, sentence_idx, pos++});
    word.clear();
  }};

  for (std::size_t offset{0}; offset < text.size();) {
    const CodePoint code_point{decode_utf8(text, offset)};
    const std::string_view bytes{text.substr(offset, code_point.len)};

    if (is_space(code_point.value)) {
      flush_word();
      if (code_point.value == '\n' || code_point.value == '\r' || code_point.value == 0x2028 ||
          code_point.value == 0x2029) {
        pending_sentence = !tokens.empty();
      }
    }
    else if (is_word_connector(code_point.value) && !word.empty() &&
             has_word_character_at(text, offset + code_point.len)) {
      word.append(bytes);
    }
    else if (is_punctuation(code_point.value)) {
      flush_word();
      tokens.push_back({std::string(bytes), std::string(bytes), TokenKind::kPunctuation, sentence_idx, pos++});
      if (is_sentence_end(code_point.value)) pending_sentence = true;
    }
    else {
      word.append(bytes);
    }

    offset += code_point.len;
  }
  flush_word();
  return tokens;
}

}  // namespace yake::detail
