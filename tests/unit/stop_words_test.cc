#include "detail/stop_words.h"

#include <gtest/gtest.h>

TEST(StopWords, LoadsSupportedEnglishNames) {
  const auto& english{yake::detail::stop_words_for_language("en")};

  EXPECT_NE(english.find("the"), english.end());
  EXPECT_NE(english.find("with"), english.end());
  EXPECT_EQ(&english, &yake::detail::stop_words_for_language("english"));
}

TEST(StopWords, RejectsUnsupportedLanguageWithoutFallback) {
  EXPECT_THROW(static_cast<void>(yake::detail::stop_words_for_language("eng")), std::invalid_argument);
  EXPECT_THROW(static_cast<void>(yake::detail::stop_words_for_language("ko")), std::invalid_argument);
}

TEST(StopWords, TreatsShortTermsAsStopWords) {
  const auto& english{yake::detail::stop_words_for_language("en")};

  EXPECT_TRUE(yake::detail::is_stop_word("of", english));
  EXPECT_TRUE(yake::detail::is_stop_word("ai", english));
  EXPECT_TRUE(yake::detail::is_stop_word("가", english));
  EXPECT_TRUE(yake::detail::is_stop_word("가나", english));
  EXPECT_FALSE(yake::detail::is_stop_word("gas", english));
}
