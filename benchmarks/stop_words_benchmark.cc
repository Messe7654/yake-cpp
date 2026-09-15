#include "detail/stop_words.h"
#include "detail/tokenizer.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

std::vector<std::string> extract_stop_words_list(const yake::detail::StopWordSet& set) {
  std::vector<std::string> list(set.begin(), set.end());
  std::sort(list.begin(), list.end());
  return list;
}

class BaselineMatcher {
 public:
  explicit BaselineMatcher(const std::vector<std::string>& words) : set_(words.begin(), words.end()) {}

  [[nodiscard]] bool contains(std::string_view token) const { return set_.find(std::string{token}) != set_.end(); }

  [[nodiscard]] static constexpr std::string_view name() noexcept { return "unordered_set (std::string)"; }

 private:
  std::unordered_set<std::string> set_{};
};

class StringViewHashSetMatcher {
 public:
  explicit StringViewHashSetMatcher(const std::vector<std::string>& words) : storage_(words) {
    for (const auto& w : storage_) {
      set_.emplace(w);
    }
  }

  [[nodiscard]] bool contains(std::string_view token) const { return set_.find(token) != set_.end(); }

  [[nodiscard]] static constexpr std::string_view name() noexcept { return "unordered_set (string_view)"; }

 private:
  std::vector<std::string> storage_{};
  std::unordered_set<std::string_view> set_{};
};

class BinarySearchMatcher {
 public:
  explicit BinarySearchMatcher(const std::vector<std::string>& words) : words_(words) {
    std::sort(words_.begin(), words_.end());
  }

  [[nodiscard]] bool contains(std::string_view token) const {
    return std::binary_search(words_.begin(), words_.end(), token);
  }

  [[nodiscard]] static constexpr std::string_view name() noexcept { return "binary_search (sorted vector)"; }

 private:
  std::vector<std::string> words_{};
};

class AhoCorasickMatcher {
 public:
  explicit AhoCorasickMatcher(const std::vector<std::string>& words) {
    nodes_.emplace_back();
    for (const auto& w : words) {
      if (w.empty()) continue;
      int curr{0};
      for (unsigned char c : w) {
        if (nodes_[curr].next[c] == -1) {
          nodes_[curr].next[c] = static_cast<int>(nodes_.size());
          nodes_.emplace_back();
        }
        curr = nodes_[curr].next[c];
      }
      nodes_[curr].is_end = true;
    }
  }

  [[nodiscard]] bool contains(std::string_view token) const {
    int curr{0};
    for (unsigned char c : token) {
      curr = nodes_[curr].next[c];
      if (curr == -1) return false;
    }
    return nodes_[curr].is_end;
  }

  [[nodiscard]] static constexpr std::string_view name() noexcept { return "aho_corasick (trie match)"; }

 private:
  struct Node {
    std::array<int, 256> next{};
    bool is_end{false};
    Node() { next.fill(-1); }
  };
  std::vector<Node> nodes_{};
};

class WuManberMatcher {
 public:
  explicit WuManberMatcher(const std::vector<std::string>& words) {
    hash_table_.resize(kTableSize);
    patterns_ = words;
    prefix_table_.resize(words.size(), 0);
    for (std::size_t idx{0}; idx < words.size(); ++idx) {
      const auto& w = words[idx];
      if (w.size() < 2) {
        if (!w.empty()) single_char_[static_cast<unsigned char>(w[0])] = true;
        continue;
      }
      const auto len   = w.size();
      const uint16_t h = (static_cast<uint8_t>(w[len - 2]) << 8) | static_cast<uint8_t>(w[len - 1]);
      hash_table_[h].push_back(static_cast<int>(idx));
      prefix_table_[idx] = (static_cast<uint8_t>(w[0]) << 8) | static_cast<uint8_t>(w[1]);
    }
  }

  [[nodiscard]] bool contains(std::string_view token) const {
    if (token.empty()) return false;
    if (token.size() == 1) {
      return single_char_[static_cast<unsigned char>(token[0])];
    }
    const auto len         = token.size();
    const uint16_t h       = (static_cast<uint8_t>(token[len - 2]) << 8) | static_cast<uint8_t>(token[len - 1]);
    const auto& candidates = hash_table_[h];
    if (candidates.empty()) return false;

    const uint16_t token_pref = (static_cast<uint8_t>(token[0]) << 8) | static_cast<uint8_t>(token[1]);
    for (int p_idx : candidates) {
      if (prefix_table_[p_idx] != token_pref) continue;
      const auto& pat = patterns_[p_idx];
      if (pat.size() == len && pat == token) return true;
    }
    return false;
  }

  [[nodiscard]] static constexpr std::string_view name() noexcept { return "wu_manber (hash + prefix)"; }

 private:
  static constexpr std::size_t kTableSize{65536};
  std::vector<std::string> patterns_{};
  std::vector<std::vector<int>> hash_table_{};
  std::vector<uint16_t> prefix_table_{};
  std::array<bool, 256> single_char_{};
};

class RabinKarpMatcher {
 public:
  explicit RabinKarpMatcher(const std::vector<std::string>& words) {
    for (const auto& w : words) {
      const uint64_t h = compute_hash(w);
      hash_map_[h].push_back(w);
    }
  }

  [[nodiscard]] bool contains(std::string_view token) const {
    const uint64_t h = compute_hash(token);
    const auto it    = hash_map_.find(h);
    if (it == hash_map_.end()) return false;
    for (const auto& pat : it->second) {
      if (pat == token) return true;
    }
    return false;
  }

  [[nodiscard]] static constexpr std::string_view name() noexcept { return "rabin_karp (polynomial hash)"; }

 private:
  static constexpr uint64_t kBase{1315423911ULL};

  static uint64_t compute_hash(std::string_view str) noexcept {
    uint64_t h{0};
    for (unsigned char c : str) {
      h = h * kBase + c;
    }
    return h;
  }

  std::unordered_map<uint64_t, std::vector<std::string>> hash_map_{};
};

template <typename Matcher>
void run_benchmark(const Matcher& matcher, const std::vector<std::string>& test_tokens, std::size_t iterations) {
  std::size_t match_count{0};

  const auto begin = std::chrono::steady_clock::now();
  for (std::size_t iter{0}; iter < iterations; ++iter) {
    for (const auto& token : test_tokens) {
      if (matcher.contains(token)) {
        ++match_count;
      }
    }
  }
  const auto end = std::chrono::steady_clock::now();

  const std::chrono::duration<double, std::nano> elapsed = end - begin;
  const std::size_t total_ops                            = iterations * test_tokens.size();
  const double ns_per_op                                 = elapsed.count() / total_ops;
  const double mops                                      = (total_ops / (elapsed.count() / 1e9)) / 1e6;

  std::cout << "  " << std::left << std::setw(32) << Matcher::name() << std::right << std::setw(10) << std::fixed
            << std::setprecision(2) << ns_per_op << " ns/op  " << std::setw(8) << std::fixed << std::setprecision(1)
            << mops << " M ops/s  "
            << "(matches: " << match_count << ")\n";
}

}  // namespace

int main() {
  const auto& stop_word_set  = yake::detail::stop_words_for_language("en");
  const auto stop_words_list = extract_stop_words_list(stop_word_set);

  std::cout << "================================================================\n";
  std::cout << "  YAKE-CPP Stop Words Lookup Benchmark\n";
  std::cout << "  Dictionary size: " << stop_words_list.size() << " stop words (English)\n";
  std::cout << "================================================================\n\n";

  const BaselineMatcher baseline(stop_words_list);
  const StringViewHashSetMatcher string_view_hash(stop_words_list);
  const BinarySearchMatcher binary_search(stop_words_list);
  const AhoCorasickMatcher aho_corasick(stop_words_list);
  const WuManberMatcher wu_manber(stop_words_list);
  const RabinKarpMatcher rabin_karp(stop_words_list);

  constexpr std::string_view kSampleText =
      "Keyword extraction identifies the most relevant expressions in a document. "
      "YAKE performs unsupervised keyword extraction using local statistical features. "
      "Graph algorithms process networks while statistical methods rank candidate phrases. "
      "It considers word frequency, casing, position in text, and contextual relatedness. "
      "Stop words like the, of, in, and, to, is, are, with, for are often filtered out "
      "to ensure that the extracted candidate keyphrases are meaningful and informative.";

  const auto tokens = yake::detail::tokenize(kSampleText);
  std::vector<std::string> realistic_tokens{};
  realistic_tokens.reserve(tokens.size());
  for (const auto& t : tokens) {
    if (t.kind == yake::detail::TokenKind::kWord) {
      realistic_tokens.push_back(t.norm);
    }
  }

  const std::vector<std::string> hit_tokens  = stop_words_list;
  const std::vector<std::string> miss_tokens = {
      "algorithm", "extraction", "document",   "unsupervised", "frequency",  "candidate", "information", "statistical",
      "keyword",   "contextual", "casing",     "sentence",     "dispersion", "graph",     "phrase",      "meaningful",
      "natural",   "language",   "processing", "machine",      "learning",   "vector"};

  for (const auto& token : realistic_tokens) {
    const bool expected = baseline.contains(token);
    const bool sv_res   = string_view_hash.contains(token);
    const bool bs_res   = binary_search.contains(token);
    const bool ac_res   = aho_corasick.contains(token);
    const bool wm_res   = wu_manber.contains(token);
    const bool rk_res   = rabin_karp.contains(token);

    if (sv_res != expected || bs_res != expected || ac_res != expected || wm_res != expected || rk_res != expected) {
      std::cerr << "ERROR: Correctness check failed for token: '" << token << "'\n";
      return 1;
    }
  }
  for (const auto& token : miss_tokens) {
    const bool expected = baseline.contains(token);
    if (string_view_hash.contains(token) != expected || binary_search.contains(token) != expected ||
        aho_corasick.contains(token) != expected || wu_manber.contains(token) != expected ||
        rabin_karp.contains(token) != expected) {
      std::cerr << "ERROR: Correctness check failed for miss token: " << token << '\n';
      return 1;
    }
  }
  std::cout << "[Verification Passed] All 6 algorithms produce 100% identical results.\n\n";

  constexpr std::size_t kIterations{10000};

  std::cout << "Test 1: Realistic Document Tokens (Stopwords + Content Words mixed, " << realistic_tokens.size()
            << " tokens)\n";
  run_benchmark(baseline, realistic_tokens, kIterations);
  run_benchmark(string_view_hash, realistic_tokens, kIterations);
  run_benchmark(binary_search, realistic_tokens, kIterations);
  run_benchmark(aho_corasick, realistic_tokens, kIterations);
  run_benchmark(wu_manber, realistic_tokens, kIterations);
  run_benchmark(rabin_karp, realistic_tokens, kIterations);

  std::cout << "\nTest 2: Pure Stop Words Hit Case (174 stop words)\n";
  run_benchmark(baseline, hit_tokens, kIterations);
  run_benchmark(string_view_hash, hit_tokens, kIterations);
  run_benchmark(binary_search, hit_tokens, kIterations);
  run_benchmark(aho_corasick, hit_tokens, kIterations);
  run_benchmark(wu_manber, hit_tokens, kIterations);
  run_benchmark(rabin_karp, hit_tokens, kIterations);

  std::cout << "\nTest 3: Pure Non-Stop Words Miss Case (" << miss_tokens.size() << " vocabulary words)\n";
  run_benchmark(baseline, miss_tokens, kIterations);
  run_benchmark(string_view_hash, miss_tokens, kIterations);
  run_benchmark(binary_search, miss_tokens, kIterations);
  run_benchmark(aho_corasick, miss_tokens, kIterations);
  run_benchmark(wu_manber, miss_tokens, kIterations);
  run_benchmark(rabin_karp, miss_tokens, kIterations);

  std::cout << "\nTest 4: YAKE Pipeline is_stop_word() (Length Filter + Dictionary Lookup)\n";

  struct PipelineBaseline {
    const yake::detail::StopWordSet& set;
    [[nodiscard]] bool is_stop(std::string_view token) const { return yake::detail::is_stop_word(token, set); }
    static constexpr std::string_view name() noexcept { return "Current is_stop_word (unordered_set)"; }
  } pipe_base{stop_word_set};

  struct PipelineAhoCorasick {
    const AhoCorasickMatcher& ac;
    [[nodiscard]] bool is_stop(std::string_view token) const {
      if (token.size() < 3) return true;
      return ac.contains(token);
    }
    static constexpr std::string_view name() noexcept { return "Pipeline + Aho-Corasick"; }
  } pipe_ac{aho_corasick};

  struct PipelineWuManber {
    const WuManberMatcher& wm;
    [[nodiscard]] bool is_stop(std::string_view token) const {
      if (token.size() < 3) return true;
      return wm.contains(token);
    }
    static constexpr std::string_view name() noexcept { return "Pipeline + Wu-Manber"; }
  } pipe_wm{wu_manber};

  struct PipelineStringViewHash {
    const StringViewHashSetMatcher& sv;
    [[nodiscard]] bool is_stop(std::string_view token) const {
      if (token.size() < 3) return true;
      return sv.contains(token);
    }
    static constexpr std::string_view name() noexcept { return "Pipeline + string_view Hash"; }
  } pipe_sv{string_view_hash};

  const auto run_pipe_bench = [&](const auto& pipe) {
    std::size_t match_count{0};
    const auto begin = std::chrono::steady_clock::now();
    for (std::size_t iter{0}; iter < kIterations; ++iter) {
      for (const auto& token : realistic_tokens) {
        if (pipe.is_stop(token)) ++match_count;
      }
    }
    const auto end                                         = std::chrono::steady_clock::now();
    const std::chrono::duration<double, std::nano> elapsed = end - begin;
    const std::size_t total_ops                            = kIterations * realistic_tokens.size();
    std::cout << "  " << std::left << std::setw(36) << pipe.name() << std::right << std::setw(10) << std::fixed
              << std::setprecision(2) << elapsed.count() / total_ops << " ns/op  " << std::setw(8) << std::fixed
              << std::setprecision(1) << (total_ops / (elapsed.count() / 1e9)) / 1e6 << " M ops/s  "
              << "(matches: " << match_count << ")\n";
  };

  run_pipe_bench(pipe_base);
  run_pipe_bench(pipe_sv);
  run_pipe_bench(pipe_wm);
  run_pipe_bench(pipe_ac);

  return 0;
}
