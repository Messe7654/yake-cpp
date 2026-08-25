#ifndef YAKE_CPP_SRC_DETAIL_TOKENIZER_H_
#define YAKE_CPP_SRC_DETAIL_TOKENIZER_H_

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace yake::detail {

enum class TokenKind { kWord, kPunctuation };

struct Token {
  std::string text = {};
  std::string norm = {};

  TokenKind kind = TokenKind::kWord;

  std::size_t sentence_idx = 0;
  std::size_t pos          = 0;
};

[[nodiscard]] std::vector<Token> tokenize(std::string_view text);

}  // namespace yake::detail

#endif  // YAKE_CPP_SRC_DETAIL_TOKENIZER_H_
