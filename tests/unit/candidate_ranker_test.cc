#include "detail/candidate_generator.h"
#include "detail/candidate_ranker.h"
#include "detail/word_features.h"

#include <gtest/gtest.h>
#include <string>
#include <unordered_map>
#include <vector>

TEST(CandidateRanker, CalculatesAndSortsCandidateScores) {
  yake::detail::CandidateMap candidates{};
  candidates.emplace("alpha", yake::detail::Candidate{"alpha", "alpha", {"alpha"}, 2, 0});
  candidates.emplace("alpha beta", yake::detail::Candidate{"alpha beta", "alpha beta", {"alpha", "beta"}, 1, 0});

  std::unordered_map<std::string, yake::detail::WordFeatures> features{};
  features.emplace("alpha", yake::detail::WordFeatures{0, 0, 0, 0, 0, 0.2});
  features.emplace("beta", yake::detail::WordFeatures{0, 0, 0, 0, 0, 0.4});

  const auto ranked{yake::detail::rank_candidates(candidates, features)};

  ASSERT_EQ(ranked.size(), 2U);
  EXPECT_EQ(ranked[0].norm, "alpha beta");
  EXPECT_NEAR(ranked[0].score, (0.2 * 0.4) / ((0.2 + 0.4 + 1.0) * 1), 1e-12);
  EXPECT_EQ(ranked[1].norm, "alpha");
  EXPECT_NEAR(ranked[1].score, 0.2 / ((0.2 + 1.0) * 2), 1e-12);
}

TEST(CandidateRanker, SkipsCandidatesWithoutCompleteFeatures) {
  yake::detail::CandidateMap candidates{};
  candidates.emplace("alpha beta", yake::detail::Candidate{"alpha beta", "alpha beta", {"alpha", "beta"}, 1, 0});

  std::unordered_map<std::string, yake::detail::WordFeatures> features{};
  features.emplace("alpha", yake::detail::WordFeatures{0, 0, 0, 0, 0, 0.2});

  EXPECT_TRUE(yake::detail::rank_candidates(candidates, features).empty());
}

TEST(CandidateRanker, WeightsInternalStopWordsUsingNeighborTransitions) {
  yake::detail::CandidateMap candidates{};
  candidates.emplace("alpha of beta",
                     yake::detail::Candidate{"alpha of beta", "alpha of beta", {"alpha", "of", "beta"}, 1, 0});

  std::unordered_map<std::string, yake::detail::WordFeatures> features{};
  features.emplace("alpha", yake::detail::WordFeatures{0, 0, 0, 0, 0, 0.2});
  features.emplace("of", yake::detail::WordFeatures{0, 0, 0, 0, 0, 10.0});
  features.emplace("beta", yake::detail::WordFeatures{0, 0, 0, 0, 0, 0.4});

  yake::detail::DocumentStatistics doc{};
  doc.words["alpha"].freq          = 4;
  doc.words["alpha"].right_context = {{"of", 2}};
  doc.words["of"].freq             = 5;
  doc.words["of"].right_context    = {{"beta", 3}};
  doc.words["beta"].freq           = 6;
  const yake::detail::StopWordSet stop_words{"of"};

  const auto ranked{yake::detail::rank_candidates(candidates, features, doc, stop_words)};

  ASSERT_EQ(ranked.size(), 1U);
  const double transition_prob{(2.0 / 4.0) * (3.0 / 6.0)};
  const double adjustment{1.0 - transition_prob};
  const double expected_score{(0.2 * (1.0 + adjustment) * 0.4) / (0.2 - adjustment + 0.4 + 1.0)};
  EXPECT_NEAR(ranked.front().score, expected_score, 1e-12);
}
