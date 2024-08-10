#pragma once

#include "../utility/units.hpp"

namespace atum {
struct Position {
  tile_t x{0_tile};
  tile_t y{0_tile};
  degree_t h{0_deg};
  meters_per_second_t v{0_fps};
  radians_per_second_t w{0_deg_per_s};
  second_t t{0_s};
  Position operator+(const Position &rhs) const;
  Position operator-(const Position &rhs) const;
};

Position operator*(const double lhs, const Position &rhs);

std::string toString(const Position &position);

tile_t distance(const Position &a, const Position &b);

degree_t angle(const Position &state, const Position &reference);

degree_t constrain180(const degree_t angle);

// Unwraps a Position or Position into raw floating point values.
// Everything here is in meters, radians, and seconds.
struct RawPosition {
  RawPosition(const double iX,
              const double iY,
              const double iH,
              const double iV,
              const double iW,
              const double iT = 0);
  RawPosition(const Position &motionPosition);
  double x;
  double y;
  double h;
  double v;
  double w;
  double t;
  RawPosition operator+(const RawPosition &rhs) const;
  RawPosition operator-(const RawPosition &rhs) const;
};

RawPosition operator*(const double lhs, const RawPosition &rhs);

std::string toString(const RawPosition &position);

double distance(const RawPosition &a, const RawPosition &b);

double angle(const RawPosition &state, const RawPosition &reference);

double constrain180(const double angle);
} // namespace atum