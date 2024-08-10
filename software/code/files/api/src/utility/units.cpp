#include "units.hpp"

namespace atum {
template <>
degree_t average<degree_t>(const std::vector<degree_t> &angles) {
  scalar_t sinSum{0};
  scalar_t cosSum{0};
  for(degree_t angle : angles) {
    sinSum += sin(angle);
    cosSum += cos(angle);
  }
  return atan2(sinSum / angles.size(), cosSum / angles.size());
}
} // namespace atum