#include "detail/tokenizer.h"
#include "detail/word_statistics.h"

#include <gtest/gtest.h>

TEST(WordStatistics, CollectsFrequencySentenceAndCaseInformation) {
  const auto tokens{yake::detail::tokenize("NASA builds Tools. NASA builds tools.")};
  const auto doc{yake::detail::collect_word_statistics(tokens, 1)};

  ASSERT_EQ(doc.word_count, 6U);
  ASSERT_EQ(doc.sentence_count, 2U);

  const auto& nasa{doc.words.at("nasa")};
  EXPECT_EQ(nasa.freq, 2U);
  EXPECT_EQ(nasa.acronym_freq, 2U);
  EXPECT_EQ(nasa.capitalized_freq, 0U);
  EXPECT_EQ(nasa.sentence_pos, (std::vector<std::size_t>{0, 1}));

  const auto& tool{doc.words.at("tool")};
  EXPECT_EQ(tool.freq, 2U);
  EXPECT_EQ(tool.capitalized_freq, 1U);
}

TEST(WordStatistics, CollectsDirectionalContextWithinWindow) {
  const auto tokens{yake::detail::tokenize("alpha beta gamma alpha")};
  const auto doc{yake::detail::collect_word_statistics(tokens, 2)};
  const auto& beta{doc.words.at("beta")};

  EXPECT_EQ(beta.left_context.at("alpha"), 1U);
  EXPECT_EQ(beta.right_context.at("gamma"), 1U);
  EXPECT_EQ(beta.right_context.at("alpha"), 1U);
}

TEST(WordStatistics, StopsContextAtPunctuation) {
  const auto tokens{yake::detail::tokenize("alpha, beta. gamma")};
  const auto doc{yake::detail::collect_word_statistics(tokens, 2)};

  EXPECT_TRUE(doc.words.at("alpha").right_context.empty());
  EXPECT_TRUE(doc.words.at("beta").left_context.empty());
  EXPECT_TRUE(doc.words.at("beta").right_context.empty());
  EXPECT_TRUE(doc.words.at("gamma").left_context.empty());
}

TEST(WordStatistics, GroupsUnicodeEquivalentForms) {
  const auto tokens{yake::detail::tokenize("CAF\xC3\x89 caf\xC3\xA9")};
  const auto doc{yake::detail::collect_word_statistics(tokens, 1)};

  ASSERT_EQ(doc.words.size(), 1U);
  EXPECT_EQ(doc.words.at("caf\xC3\xA9").freq, 2U);
}

TEST(WordStatistics, GroupsSimpleEnglishPluralForms) {
  const auto tokens{yake::detail::tokenize("graph graphs algorithm algorithms")};
  const auto doc{yake::detail::collect_word_statistics(tokens, 1)};

  EXPECT_EQ(doc.words.size(), 2U);
  EXPECT_EQ(doc.words.at("graph").freq, 2U);
  EXPECT_EQ(doc.words.at("algorithm").freq, 2U);
}
