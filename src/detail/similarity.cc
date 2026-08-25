#include "detail/similarity.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <utf8proc.h>
#include <utility>
#include <vector>

namespace yake::detail {
namespace {

using CodePoints = std::vector<std::uint32_t>;

CodePoints decode_code_points(std::string_view text) {
  CodePoints points{};
  const auto* data{reinterpret_cast<const utf8proc_uint8_t*>(text.data())};

  for (std::size_t offset{0}; offset < text.size();) {
    utf8proc_int32_t point{0};
    const utf8proc_ssize_t len{
        utf8proc_iterate(data + offset, static_cast<utf8proc_ssize_t>(text.size() - offset), &point)};
    if (len < 0) {
      points.push_back(static_cast<unsigned char>(text[offset]));
      ++offset;
      continue;
    }

    points.push_back(static_cast<std::uint32_t>(point));
    offset += static_cast<std::size_t>(len);
  }
  return points;
}

std::size_t count_sequence_matches(const CodePoints& lhs, std::size_t lhs_begin, std::size_t lhs_end,
                                   const CodePoints& rhs, std::size_t rhs_begin, std::size_t rhs_end) {
  std::size_t best_len{0};
  std::size_t best_lhs{lhs_begin};
  std::size_t best_rhs{rhs_begin};
  std::vector<std::size_t> prev(rhs_end - rhs_begin + 1);
  std::vector<std::size_t> curr(rhs_end - rhs_begin + 1);

  for (std::size_t lhs_idx{lhs_begin}; lhs_idx < lhs_end; ++lhs_idx) {
    for (std::size_t rhs_idx{rhs_begin}; rhs_idx < rhs_end; ++rhs_idx) {
      const std::size_t col{rhs_idx - rhs_begin + 1};
      curr[col] = (lhs[lhs_idx] == rhs[rhs_idx]) ? prev[col - 1] + 1 : 0;
      if (curr[col] > best_len) {
        best_len = curr[col];
        best_lhs = lhs_idx + 1 - best_len;
        best_rhs = rhs_idx + 1 - best_len;
      }
    }
    prev.swap(curr);
    std::fill(curr.begin(), curr.end(), 0);
  }

  if (best_len == 0) return 0;
  return best_len + count_sequence_matches(lhs, lhs_begin, best_lhs, rhs, rhs_begin, best_rhs) +
         count_sequence_matches(lhs, best_lhs + best_len, lhs_end, rhs, best_rhs + best_len, rhs_end);
}

}  // namespace

double levenshtein_similarity(std::string_view lhs, std::string_view rhs) {
  const auto lhs_points{decode_code_points(lhs)};
  const auto rhs_points{decode_code_points(rhs)};
  if (lhs_points.empty() && rhs_points.empty()) return 1.0;

  const CodePoints* shorter{&lhs_points};
  const CodePoints* longer{&rhs_points};
  if (shorter->size() > longer->size()) std::swap(shorter, longer);

  std::vector<std::size_t> prev(shorter->size() + 1);
  std::vector<std::size_t> curr(shorter->size() + 1);
  for (std::size_t col{0}; col < prev.size(); ++col) prev[col] = col;

  for (std::size_t row{1}; row <= longer->size(); ++row) {
    curr[0] = row;
    for (std::size_t col{1}; col <= shorter->size(); ++col) {
      const std::size_t sub_cost{((*longer)[row - 1] == (*shorter)[col - 1]) ? 0U : 1U};
      curr[col] = std::min({prev[col] + 1, curr[col - 1] + 1, prev[col - 1] + sub_cost});
    }
    prev.swap(curr);
  }
  return 1.0 - static_cast<double>(prev.back()) / longer->size();
}

double jaro_winkler_similarity(std::string_view lhs, std::string_view rhs) {
  const auto lhs_points{decode_code_points(lhs)};
  const auto rhs_points{decode_code_points(rhs)};
  if (lhs_points == rhs_points) return 1.0;
  if (lhs_points.empty() || rhs_points.empty()) return 0.0;

  const std::size_t range{std::max(lhs_points.size(), rhs_points.size()) / 2};
  const std::size_t window{(range == 0) ? 0 : range - 1};
  std::vector<bool> lhs_matched(lhs_points.size());
  std::vector<bool> rhs_matched(rhs_points.size());
  std::size_t matches{0};

  for (std::size_t lhs_idx{0}; lhs_idx < lhs_points.size(); ++lhs_idx) {
    const std::size_t begin{(lhs_idx > window) ? lhs_idx - window : 0};
    const std::size_t end{std::min(lhs_idx + window + 1, rhs_points.size())};
    for (std::size_t rhs_idx{begin}; rhs_idx < end; ++rhs_idx) {
      if (rhs_matched[rhs_idx] || lhs_points[lhs_idx] != rhs_points[rhs_idx]) continue;
      lhs_matched[lhs_idx] = true;
      rhs_matched[rhs_idx] = true;
      ++matches;
      break;
    }
  }
  if (matches == 0) return 0.0;

  std::size_t rhs_idx{0};
  std::size_t transpositions{0};
  for (std::size_t lhs_idx{0}; lhs_idx < lhs_points.size(); ++lhs_idx) {
    if (!lhs_matched[lhs_idx]) continue;
    while (!rhs_matched[rhs_idx]) ++rhs_idx;
    if (lhs_points[lhs_idx] != rhs_points[rhs_idx]) ++transpositions;
    ++rhs_idx;
  }

  const double match_count{static_cast<double>(matches)};
  const double jaro{(match_count / lhs_points.size() + match_count / rhs_points.size() +
                     (match_count - static_cast<double>(transpositions) / 2) / match_count) /
                    3};
  if (jaro <= 0.7) return jaro;

  std::size_t prefix{0};
  while (prefix < std::min({std::size_t{4}, lhs_points.size(), rhs_points.size()}) &&
         lhs_points[prefix] == rhs_points[prefix])
    ++prefix;
  return jaro + static_cast<double>(prefix) * 0.1 * (1.0 - jaro);
}

double sequence_matcher_similarity(std::string_view lhs, std::string_view rhs) {
  const auto lhs_points{decode_code_points(lhs)};
  const auto rhs_points{decode_code_points(rhs)};
  if (lhs_points.empty() && rhs_points.empty()) return 1.0;

  const std::size_t matches{count_sequence_matches(lhs_points, 0, lhs_points.size(), rhs_points, 0, rhs_points.size())};
  return 2.0 * matches / (lhs_points.size() + rhs_points.size());
}

}  // namespace yake::detail
