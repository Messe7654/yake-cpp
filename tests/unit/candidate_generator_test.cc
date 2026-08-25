#include "detail/candidate_generator.h"
#include "detail/stop_words.h"
#include "detail/tokenizer.h"

#include <gtest/gtest.h>

TEST(CandidateGenerator, GeneratesAllNgramsUpToLimit) {
  const auto tokens{yake::detail::tokenize("one two three")};
  const auto candidates{yake::detail::generate_candidates(tokens, 2)};

  EXPECT_EQ(candidates.size(), 5U);
  EXPECT_EQ(candidates.at("one").terms, (std::vector<std::string>{"one"}));
  EXPECT_EQ(candidates.at("one two").terms, (std::vector<std::string>{"one", "two"}));
  EXPECT_EQ(candidates.at("two three").freq, 1U);
  EXPECT_EQ(candidates.at("three").first_pos, 2U);
  EXPECT_EQ(candidates.find("one two three"), candidates.end());
}

TEST(CandidateGenerator, StopsAtPunctuationAndSentenceBoundaries) {
  const auto tokens{yake::detail::tokenize("one two, three. four five")};
  const auto candidates{yake::detail::generate_candidates(tokens, 3)};

  EXPECT_NE(candidates.find("one two"), candidates.end());
  EXPECT_EQ(candidates.find("two three"), candidates.end());
  EXPECT_EQ(candidates.find("three four"), candidates.end());
  EXPECT_NE(candidates.find("four five"), candidates.end());
}

TEST(CandidateGenerator, GroupsNormalizedFormsAndSelectsMostFrequentText) {
  const auto tokens{yake::detail::tokenize("Deep Learning. deep learning. deep learning. DEEP learning.")};
  const auto candidates{yake::detail::generate_candidates(tokens, 2)};
  const auto& cand{candidates.at("deep learning")};

  EXPECT_EQ(cand.freq, 4U);
  EXPECT_EQ(cand.text, "deep learning");
  EXPECT_EQ(cand.text_frequencies.at("Deep Learning"), 1U);
  EXPECT_EQ(cand.text_frequencies.at("deep learning"), 2U);
  EXPECT_EQ(cand.text_frequencies.at("DEEP learning"), 1U);
}

TEST(CandidateGenerator, HandlesZeroLimitAndEmptyInput) {
  const auto tokens{yake::detail::tokenize("one two")};

  EXPECT_TRUE(yake::detail::generate_candidates(tokens, 0).empty());
  EXPECT_TRUE(yake::detail::generate_candidates({}, 3).empty());
}

TEST(CandidateGenerator, RejectsCandidatesWithStopWordsAtTheirEdges) {
  const auto tokens{yake::detail::tokenize("the theory of learning in systems")};
  const auto& stop_words{yake::detail::stop_words_for_language("en")};
  const auto candidates{yake::detail::generate_candidates(tokens, 3, stop_words)};

  EXPECT_EQ(candidates.find("the"), candidates.end());
  EXPECT_EQ(candidates.find("the theory"), candidates.end());
  EXPECT_EQ(candidates.find("theory of"), candidates.end());
  EXPECT_NE(candidates.find("theory of learning"), candidates.end());
  EXPECT_NE(candidates.find("learning in systems"), candidates.end());
  EXPECT_EQ(candidates.find("in systems"), candidates.end());
  EXPECT_NE(candidates.find("systems"), candidates.end());
}
