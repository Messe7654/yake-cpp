#include "detail/tokenizer.h"
#include "detail/word_features.h"
#include "detail/word_statistics.h"

#include <cmath>
#include <gtest/gtest.h>

TEST(WordFeatures, CalculatesOfficialYakeFormula) {
  yake::detail::DocumentStatistics doc{};
  doc.sentence_count = 4;
  doc.word_count     = 6;

  yake::detail::WordStatistics alpha{};
  alpha.norm             = "alpha";
  alpha.freq             = 2;
  alpha.capitalized_freq = 1;
  alpha.sentence_pos     = {0, 2};
  alpha.left_context     = {{"x", 2}, {"y", 1}};
  alpha.right_context    = {{"z", 2}};
  doc.words.emplace(alpha.norm, alpha);

  yake::detail::WordStatistics beta{};
  beta.norm         = "beta";
  beta.freq         = 4;
  beta.sentence_pos = {0, 1, 2, 3};
  doc.words.emplace(beta.norm, beta);

  const auto features{yake::detail::calculate_word_features(doc)};
  const auto& alpha_features{features.at("alpha")};

  const double expected_relatedness{(0.5 + (2.0 / 3.0) * 0.5) + (0.5 + 0.5 * 0.5)};
  const double expected_freq{2.0 / (3.0 + 1.0)};
  const double expected_dispersion{2.0 / 4.0};
  const double expected_casing{1.0 / (1.0 + std::log(2.0))};
  const double expected_pos{std::log(std::log(4.0))};
  const double expected_score{
      (expected_pos * expected_relatedness) /
      (expected_casing + expected_freq / expected_relatedness + expected_dispersion / expected_relatedness)};

  EXPECT_NEAR(alpha_features.relatedness, expected_relatedness, 1e-12);
  EXPECT_NEAR(alpha_features.freq, expected_freq, 1e-12);
  EXPECT_NEAR(alpha_features.sentence_dispersion, expected_dispersion, 1e-12);
  EXPECT_NEAR(alpha_features.casing, expected_casing, 1e-12);
  EXPECT_NEAR(alpha_features.pos, expected_pos, 1e-12);
  EXPECT_NEAR(alpha_features.score, expected_score, 1e-12);
}

TEST(WordFeatures, ProducesFiniteScoresFromCollectedStatistics) {
  const auto tokens{yake::detail::tokenize("YAKE extracts keywords. Keywords describe text.")};
  const auto doc{yake::detail::collect_word_statistics(tokens, 1)};
  const auto features{yake::detail::calculate_word_features(doc)};

  ASSERT_EQ(features.size(), doc.words.size());
  for (const auto& [word, values] : features) {
    EXPECT_FALSE(word.empty());
    EXPECT_TRUE(std::isfinite(values.score));
    EXPECT_GT(values.relatedness, 0.0);
  }
}

TEST(WordFeatures, HandlesEmptyDocument) { EXPECT_TRUE(yake::detail::calculate_word_features({}).empty()); }

TEST(WordFeatures, ExcludesStopWordsFromFrequencyDistribution) {
  yake::detail::DocumentStatistics doc{};
  doc.sentence_count = 1;

  yake::detail::WordStatistics alpha{};
  alpha.freq         = 2;
  alpha.sentence_pos = {0};
  doc.words.emplace("alpha", alpha);

  yake::detail::WordStatistics beta{};
  beta.freq         = 4;
  beta.sentence_pos = {0};
  doc.words.emplace("beta", beta);

  yake::detail::WordStatistics stop_word{};
  stop_word.freq         = 100;
  stop_word.sentence_pos = {0};
  doc.words.emplace("the", stop_word);

  const yake::detail::StopWordSet stop_words{"the"};
  const auto features{yake::detail::calculate_word_features(doc, stop_words)};

  EXPECT_NEAR(features.at("alpha").freq, 2.0 / (3.0 + 1.0), 1e-12);
  EXPECT_NEAR(features.at("beta").freq, 4.0 / (3.0 + 1.0), 1e-12);
  EXPECT_NEAR(features.at("the").freq, 100.0 / (3.0 + 1.0), 1e-12);
}
