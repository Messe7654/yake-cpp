#include "detail/candidate_generator.h"
#include "detail/term_normalizer.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace yake::detail {
namespace {

void append_term(std::string& phrase, std::string_view term) {
  if (!phrase.empty()) phrase.push_back(' ');
  phrase.append(term);
}

}  // namespace

CandidateMap generate_candidates(const std::vector<Token>& tokens, std::size_t max_ngram_size,
                                 const StopWordSet& stop_words) {
  CandidateMap candidates{};
  if (max_ngram_size == 0) return candidates;

  std::size_t word_pos{0};
  for (std::size_t start{0}; start < tokens.size(); ++start) {
    const Token& first{tokens[start]};
    if (first.kind != TokenKind::kWord) continue;

    std::string text{};
    std::string norm{};
    std::vector<std::string> terms{};
    terms.reserve(max_ngram_size);

    std::size_t cursor{start};
    while (cursor < tokens.size() && terms.size() < max_ngram_size) {
      const Token& token{tokens[cursor]};
      if (token.kind != TokenKind::kWord || token.sentence_idx != first.sentence_idx) break;

      append_term(text, token.text);
      append_term(norm, token.norm);
      terms.push_back(normalize_term(token.norm));

      if (is_stop_word(first.norm, stop_words) || is_stop_word(token.norm, stop_words)) {
        ++cursor;
        continue;
      }

      auto [it, inserted]{candidates.try_emplace(norm)};
      Candidate& cand{it->second};
      if (inserted) {
        cand.text      = text;
        cand.norm      = norm;
        cand.terms     = terms;
        cand.first_pos = word_pos;
      }

      ++cand.freq;
      const std::size_t text_freq{++cand.text_frequencies[text]};
      if (text_freq > cand.selected_text_freq) {
        cand.text               = text;
        cand.selected_text_freq = text_freq;
      }

      ++cursor;
    }

    ++word_pos;
  }

  return candidates;
}

}  // namespace yake::detail
