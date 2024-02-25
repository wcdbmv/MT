#pragma once

#include <algorithm>

template <typename Container, typename Predicate>
constexpr void EraseRemoveIf(Container&& container, Predicate&& predicate) {
  const auto range = std::ranges::remove_if(std::forward<Container>(container),
                                            std::forward<Predicate>(predicate));
  container.erase(range.begin(), range.end());
}
