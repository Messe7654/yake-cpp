#include "detail/term_normalizer.h"

#include <gtest/gtest.h>

TEST(TermNormalizer, MergesSimpleEnglishPluralForms) {
  EXPECT_EQ(yake::detail::normalize_term("graph"), "graph");
  EXPECT_EQ(yake::detail::normalize_term("graphs"), "graph");
  EXPECT_EQ(yake::detail::normalize_term("algorithms"), "algorithm");
  EXPECT_EQ(yake::detail::normalize_term("gas"), "gas");
}
