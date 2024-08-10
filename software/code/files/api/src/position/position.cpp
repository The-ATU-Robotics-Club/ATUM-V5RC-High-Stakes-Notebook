#include "position.hpp"

namespace atum {
std::string toString(const Position &position) {
  return "(" + to_string(position.x) + ", " + to_string(position.y) + ", " +
         to_string(position.h) + ", " + to_string(position.v) + ", " +
         to_string(position.w) + ", " + to_string(position.t) + ")";
}

Position Position::operator+(const Position &rhs) const {
  return {x + rhs.x, y + rhs.y, h, v, w, t};
}

Position Position::operator-(const Position &rhs) const {
  return {x - rhs.x, y - rhs.y, h, v, w, t};
}

Position operator*(const double lhs, const Position &rhs) {
  return {lhs * rhs.x, lhs * rhs.y, rhs.h, rhs.v, rhs.w, rhs.t};
}

tile_t distance(const Position &a, const Position &b) {
  return sqrt(pow<2>(a.x - b.x) + pow<2>(a.y - b.y));
}

degree_t angle(const Position &state, const Position &reference) {
  const inch_t dx{reference.x - state.x};
  const inch_t dy{reference.y - state.y};
  const degree_t dh{90_deg - atan2(dy, dx)};
  return constrain180(dh);
}

degree_t constrain180(const degree_t angle) {
  return degree_t{remainder(getValueAs<degree_t>(angle), 360.0)};
}

RawPosition::RawPosition(const double iX,
                         const double iY,
                         const double iH,
                         const double iV,
                         const double iW,
                         const double iT) :
    x{iX}, y{iY}, h{iH}, v{iV}, w{iW}, t{iT} {}

RawPosition::RawPosition(const Position &position) :
    x{getValueAs<meter_t>(position.x)},
    y{getValueAs<meter_t>(position.y)},
    h{getValueAs<radian_t>(position.h)},
    v{getValueAs<meters_per_second_t>(position.v)},
    w{getValueAs<radians_per_second_t>(position.w)},
    t{getValueAs<second_t>(position.t)} {}

RawPosition RawPosition::operator+(const RawPosition &rhs) const {
  return {x + rhs.x, y + rhs.y, h, v, w, t};
}

RawPosition RawPosition::operator-(const RawPosition &rhs) const {
  return {x - rhs.x, y - rhs.y, constrain180(h - rhs.h), v, w, t};
}

RawPosition operator*(const double lhs, const RawPosition &rhs) {
  return {lhs * rhs.x, lhs * rhs.y, rhs.h, rhs.v, rhs.w, rhs.t};
}

std::string toString(const RawPosition &rawPosition) {
  return "(" + std::to_string(rawPosition.x) + " m, " +
         std::to_string(rawPosition.y) + " m, " +
         std::to_string(rawPosition.h) + " rad, " +
         std::to_string(rawPosition.v) + " m/s, " +
         std::to_string(rawPosition.w) + " rad/s, " +
         std::to_string(rawPosition.t) + " s)";
}

double distance(const RawPosition &a, const RawPosition &b) {
  return sqrt(pow(a.x - b.x, 2.0) + pow(a.y - b.y, 2.0));
}

double angle(const RawPosition &state, const RawPosition &reference) {
  const double dx{reference.x - state.x};
  const double dy{reference.y - state.y};
  const double dh{M_PI_2 - atan2(dy, dx)};
  return constrain180(dh);
}

double constrain180(const double angle) {
  return remainder(angle, 2.0 * M_PI);
}

} // namespace atum