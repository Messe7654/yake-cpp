#include "detail/term_normalizer.h"

#include <string>
#include <string_view>

namespace yake::detail {

std::string normalize_term(std::string_view token) {
  std::string term{token};
  if (term.size() > 3 && term.back() == 's') term.pop_back();
  return term;
}

}  // namespace yake::detail
