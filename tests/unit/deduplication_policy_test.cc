#include "yake/deduplication_policy.h"

#include <gtest/gtest.h>

TEST(LevenshteinPolicy, CalculatesNormalizedEditSimilarity) {
  const yake::LevenshteinPolicy policy{};

  EXPECT_NEAR(policy.similarity("kitten", "sitting"), 4.0 / 7.0, 1e-12);
  EXPECT_DOUBLE_EQ(policy.similarity("키워드", "키워드"), 1.0);
}

TEST(JaroWinklerPolicy, RewardsCommonPrefixes) {
  const yake::JaroWinklerPolicy policy{};

  EXPECT_NEAR(policy.similarity("MARTHA", "MARHTA"), 0.9611111111111111, 1e-12);
  EXPECT_DOUBLE_EQ(policy.similarity("", "text"), 0.0);
}

TEST(SequenceMatcherPolicy, CalculatesMatchingBlockRatio) {
  const yake::SequenceMatcherPolicy policy{};

  EXPECT_DOUBLE_EQ(policy.similarity("abcd", "abxd"), 0.75);
  EXPECT_DOUBLE_EQ(policy.similarity("", ""), 1.0);
}
