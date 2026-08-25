#include "detail/normalizer.h"

#include <cstdlib>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utf8proc.h>

namespace yake::detail {
namespace {

struct Utf8procDeleter {
  void operator()(utf8proc_uint8_t* pointer) const noexcept { std::free(pointer); }
};

void replace_all(std::string& text, std::string_view from, std::string_view to) {
  std::size_t pos{0};
  while ((pos = text.find(from, pos)) != std::string::npos) {
    text.replace(pos, from.size(), to);
    pos += to.size();
  }
}

std::string canonicalize_connectors(std::string text) {
  replace_all(text, "\xE2\x80\x98", "'");  // left single quotation mark
  replace_all(text, "\xE2\x80\x99", "'");  // right single quotation mark
  replace_all(text, "\xE2\x80\x90", "-");  // hyphen
  replace_all(text, "\xE2\x80\x91", "-");  // non-breaking hyphen
  return text;
}

}  // namespace

std::string normalize_token(std::string_view token) {
  if (token.empty()) return {};
  if (token.size() > static_cast<std::size_t>(std::numeric_limits<utf8proc_ssize_t>::max()))
    throw std::length_error("token is too large to normalize");

  utf8proc_uint8_t* output{nullptr};
  constexpr auto kOptions{
      static_cast<utf8proc_option_t>(UTF8PROC_STABLE | UTF8PROC_COMPOSE | UTF8PROC_COMPAT | UTF8PROC_CASEFOLD)};
  const utf8proc_ssize_t len{utf8proc_map(reinterpret_cast<const utf8proc_uint8_t*>(token.data()),
                                          static_cast<utf8proc_ssize_t>(token.size()), &output, kOptions)};

  if (len < 0) throw std::invalid_argument(utf8proc_errmsg(len));

  const std::unique_ptr<utf8proc_uint8_t, Utf8procDeleter> norm{output};
  return canonicalize_connectors({reinterpret_cast<const char*>(norm.get()), static_cast<std::size_t>(len)});
}

}  // namespace yake::detail
