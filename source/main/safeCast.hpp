#ifndef safeCast_hpp
#define safeCast_hpp

#include <optional>
#include <limits>
#include <type_traits>

namespace Project {
  template <typename Output_T, typename Input_T>
  constexpr std::optional<Output_T> safeCast(Input_T const value) {
    static_assert(std::is_integral_v<Input_T>);
    static_assert(std::is_integral_v<Output_T>);

    static constexpr Output_T minimumLimit = std::numeric_limits<Output_T>::min();
    static constexpr Output_T maximumLimit = std::numeric_limits<Output_T>::max();

    // I'm putting my trust in implicit integer promotion!
    if constexpr (value >= minimumLimit && value <= maximumLimit)
      return value;
    else
      return std::nullopt;
  }
}

#endif
