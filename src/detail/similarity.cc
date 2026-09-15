#include "detail/similarity.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utf8proc.h>
#include <utility>
#include <vector>

namespace yake::detail {
namespace {

using CodePoints    = std::u32string;
using CodePointView = std::u32string_view;

CodePoints decode_code_points(std::string_view text) {
  CodePoints points{};
  const auto* data{reinterpret_cast<const utf8proc_uint8_t*>(text.data())};

  for (std::size_t offset{0}; offset < text.size();) {
    utf8proc_int32_t point{0};
    const utf8proc_ssize_t len{
        utf8proc_iterate(data + offset, static_cast<utf8proc_ssize_t>(text.size() - offset), &point)};
    if (len < 0) {
      points.push_back(static_cast<char32_t>(static_cast<unsigned char>(text[offset])));
      ++offset;
      continue;
    }

    points.push_back(static_cast<char32_t>(point));
    offset += static_cast<std::size_t>(len);
  }
  return points;
}

struct SequenceMatcherBuffer {
  std::vector<std::size_t> prev;
  std::vector<std::size_t> curr;

  explicit SequenceMatcherBuffer(std::size_t max_size) : prev(max_size), curr(max_size) {}
};

std::size_t count_sequence_matches(CodePointView lhs, CodePointView rhs, std::size_t* prev, std::size_t* curr) {
  std::size_t best_len{0};
  std::size_t best_lhs{0};
  std::size_t best_rhs{0};
  const std::size_t row_len{rhs.size() + 1};
  std::fill_n(prev, row_len, 0);
  std::fill_n(curr, row_len, 0);

  std::size_t lhs_idx{0};
  for (const char32_t lhs_char : lhs) {
    std::size_t col{1};
    for (const char32_t rhs_char : rhs) {
      curr[col] = (lhs_char == rhs_char) ? prev[col - 1] + 1 : 0;
      if (curr[col] > best_len) {
        best_len = curr[col];
        best_lhs = lhs_idx + 1 - best_len;
        best_rhs = col - best_len;
      }
      ++col;
    }
    std::swap(prev, curr);
    std::fill_n(curr, row_len, 0);
    ++lhs_idx;
  }

  if (best_len == 0) return 0;
  return best_len + count_sequence_matches(lhs.substr(0, best_lhs), rhs.substr(0, best_rhs), prev, curr) +
         count_sequence_matches(lhs.substr(best_lhs + best_len), rhs.substr(best_rhs + best_len), prev, curr);
}

}  // namespace

double levenshtein_similarity(std::string_view lhs, std::string_view rhs) {
  const auto lhs_points{decode_code_points(lhs)};
  const auto rhs_points{decode_code_points(rhs)};
  if (lhs_points.empty() && rhs_points.empty()) return 1.0;

  CodePointView shorter{lhs_points};
  CodePointView longer{rhs_points};
  if (shorter.size() > longer.size()) std::swap(shorter, longer);

  std::vector<std::size_t> prev(shorter.size() + 1);
  std::vector<std::size_t> curr(shorter.size() + 1);
  for (std::size_t col{0}; col < prev.size(); ++col) prev[col] = col;

  std::size_t row{1};
  for (const char32_t longer_char : longer) {
    curr[0] = row;
    std::size_t col{1};
    for (const char32_t shorter_char : shorter) {
      const std::size_t sub_cost{(longer_char == shorter_char) ? 0U : 1U};
      curr[col] = std::min({prev[col] + 1, curr[col - 1] + 1, prev[col - 1] + sub_cost});
      ++col;
    }
    prev.swap(curr);
    ++row;
  }
  return 1.0 - static_cast<double>(prev.back()) / longer.size();
}

double jaro_winkler_similarity(std::string_view lhs, std::string_view rhs) {
  const auto lhs_points{decode_code_points(lhs)};
  const auto rhs_points{decode_code_points(rhs)};
  const CodePointView lhs_view{lhs_points};
  const CodePointView rhs_view{rhs_points};
  if (lhs_view == rhs_view) return 1.0;
  if (lhs_view.empty() || rhs_view.empty()) return 0.0;

  const std::size_t range{std::max(lhs_view.size(), rhs_view.size()) / 2};
  const std::size_t window{(range == 0) ? 0 : range - 1};
  std::vector<bool> lhs_matched(lhs_view.size());
  std::vector<bool> rhs_matched(rhs_view.size());
  std::size_t matches{0};

  for (std::size_t lhs_idx{0}; lhs_idx < lhs_view.size(); ++lhs_idx) {
    const std::size_t begin{(lhs_idx > window) ? lhs_idx - window : 0};
    const std::size_t end{std::min(lhs_idx + window + 1, rhs_view.size())};
    for (std::size_t rhs_idx{begin}; rhs_idx < end; ++rhs_idx) {
      if (rhs_matched[rhs_idx] || lhs_view[lhs_idx] != rhs_view[rhs_idx]) continue;
      lhs_matched[lhs_idx] = true;
      rhs_matched[rhs_idx] = true;
      ++matches;
      break;
    }
  }
  if (matches == 0) return 0.0;

  std::size_t rhs_idx{0};
  std::size_t transpositions{0};
  for (std::size_t lhs_idx{0}; lhs_idx < lhs_view.size(); ++lhs_idx) {
    if (!lhs_matched[lhs_idx]) continue;
    while (!rhs_matched[rhs_idx]) ++rhs_idx;
    if (lhs_view[lhs_idx] != rhs_view[rhs_idx]) ++transpositions;
    ++rhs_idx;
  }

  const double match_count{static_cast<double>(matches)};
  const double jaro{(match_count / lhs_view.size() + match_count / rhs_view.size() +
                     (match_count - static_cast<double>(transpositions) / 2) / match_count) /
                    3};
  if (jaro <= 0.7) return jaro;

  std::size_t prefix{0};
  while (prefix < std::min({std::size_t{4}, lhs_view.size(), rhs_view.size()}) && lhs_view[prefix] == rhs_view[prefix])
    ++prefix;
  return jaro + static_cast<double>(prefix) * 0.1 * (1.0 - jaro);
}

double sequence_matcher_similarity(std::string_view lhs, std::string_view rhs) {
  const auto lhs_points{decode_code_points(lhs)};
  const auto rhs_points{decode_code_points(rhs)};
  if (lhs_points.empty() && rhs_points.empty()) return 1.0;
  if (lhs_points.empty() || rhs_points.empty()) return 0.0;

  SequenceMatcherBuffer buffer{rhs_points.size() + 1};
  const std::size_t matches{count_sequence_matches(lhs_points, rhs_points, buffer.prev.data(), buffer.curr.data())};
  return 2.0 * matches / (lhs_points.size() + rhs_points.size());
}

}  // namespace yake::detail
