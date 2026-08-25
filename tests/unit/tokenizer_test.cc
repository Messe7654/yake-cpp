#include "detail/tokenizer.h"

#include <gtest/gtest.h>

using yake::detail::TokenKind;

TEST(Tokenizer, SeparatesWordsAndPunctuation) {
  const auto tokens{yake::detail::tokenize("Hello, YAKE!")};

  ASSERT_EQ(tokens.size(), 4U);
  EXPECT_EQ(tokens[0].text, "Hello");
  EXPECT_EQ(tokens[0].norm, "hello");
  EXPECT_EQ(tokens[0].kind, TokenKind::kWord);
  EXPECT_EQ(tokens[1].text, ",");
  EXPECT_EQ(tokens[1].kind, TokenKind::kPunctuation);
  EXPECT_EQ(tokens[2].norm, "yake");
  EXPECT_EQ(tokens[3].text, "!");
}

TEST(Tokenizer, PreservesUtf8Words) {
  const auto tokens{yake::detail::tokenize("\xED\x82\xA4\xEC\x9B\x8C\xEB\x93\x9C \xEC\xB6\x94\xEC\xB6\x9C")};

  ASSERT_EQ(tokens.size(), 2U);
  EXPECT_EQ(tokens[0].text, "\xED\x82\xA4\xEC\x9B\x8C\xEB\x93\x9C");
  EXPECT_EQ(tokens[1].text, "\xEC\xB6\x94\xEC\xB6\x9C");
}

TEST(Tokenizer, TracksSentenceAndTokenPositions) {
  const auto tokens{yake::detail::tokenize("One. Two? Three")};

  ASSERT_EQ(tokens.size(), 5U);
  EXPECT_EQ(tokens[0].sentence_idx, 0U);
  EXPECT_EQ(tokens[0].pos, 0U);
  EXPECT_EQ(tokens[1].sentence_idx, 0U);
  EXPECT_EQ(tokens[2].sentence_idx, 1U);
  EXPECT_EQ(tokens[2].pos, 0U);
  EXPECT_EQ(tokens[4].sentence_idx, 2U);
  EXPECT_EQ(tokens[4].pos, 0U);
}

TEST(Tokenizer, RecognizesCjkSentencePunctuation) {
  const auto tokens{yake::detail::tokenize("\xEC\x95\x88\xEB\x85\x95\xE3\x80\x82\xEC\x84\xB8\xEA\xB3\x84")};

  ASSERT_EQ(tokens.size(), 3U);
  EXPECT_EQ(tokens[1].kind, TokenKind::kPunctuation);
  EXPECT_EQ(tokens[2].sentence_idx, 1U);
}

TEST(Tokenizer, KeepsConnectorsInsideWords) {
  const auto tokens{yake::detail::tokenize("Don't use state-of-the-art's label - loosely.")};

  ASSERT_EQ(tokens.size(), 7U);
  EXPECT_EQ(tokens[0].text, "Don't");
  EXPECT_EQ(tokens[0].norm, "don't");
  EXPECT_EQ(tokens[2].text, "state-of-the-art's");
  EXPECT_EQ(tokens[3].text, "label");
  EXPECT_EQ(tokens[4].text, "-");
  EXPECT_EQ(tokens[4].kind, TokenKind::kPunctuation);
}

TEST(Tokenizer, NormalizesUnicodeConnectorsAndKeepsNumbers) {
  const auto tokens{yake::detail::tokenize("YAKE\xE2\x80\x99s 2\xE2\x80\x91gram")};

  ASSERT_EQ(tokens.size(), 2U);
  EXPECT_EQ(tokens[0].norm, "yake's");
  EXPECT_EQ(tokens[1].norm, "2-gram");
}
