#include "pose.hpp"

namespace atum {
UnwrappedPose::UnwrappedPose(const double iX,
                             const double iY,
                             const double iH,
                             const double iV,
                             const double iA,
                             const double iOmega,
                             const double iAlpha,
                             const double iT) :
    x{iX}, y{iY}, h{iH}, v{iV}, a{iA}, omega{iOmega}, alpha{iAlpha}, t{iT} {}

UnwrappedPose::UnwrappedPose(const Pose &pose) :
    x{getValueAs<meter_t>(pose.x)},
    y{getValueAs<meter_t>(pose.y)},
    h{getValueAs<radian_t>(pose.h)},
    v{getValueAs<meters_per_second_t>(pose.v)},
    a{getValueAs<meters_per_second_squared_t>(pose.a)},
    omega{getValueAs<radians_per_second_t>(pose.omega)},
    alpha{getValueAs<radians_per_second_squared_t>(pose.alpha)},
    t{getValueAs<second_t>(pose.t)} {}

bool UnwrappedPose::operator==(const UnwrappedPose &rhs) const {
  return x == rhs.x && y == rhs.y && h == rhs.h;
}

bool UnwrappedPose::operator!=(const UnwrappedPose &rhs) const {
  return !(*this == rhs);
}

UnwrappedPose UnwrappedPose::operator+(const UnwrappedPose &rhs) const {
  return {x + rhs.x, y + rhs.y};
}

UnwrappedPose UnwrappedPose::operator-(const UnwrappedPose &rhs) const {
  return {x - rhs.x, y - rhs.y};
}

void UnwrappedPose::flip() {
  x *= -1;
  h *= -1;
}

UnwrappedPose operator*(const double lhs, const UnwrappedPose &rhs) {
  return {lhs * rhs.x, lhs * rhs.y};
}

UnwrappedPose operator*(const UnwrappedPose &lhs, const double rhs) {
  return {lhs.x * rhs, lhs.y * rhs};
}

double distance(const UnwrappedPose &a, const UnwrappedPose &b) {
  return std::sqrt(std::pow(a.x - b.x, 2.0) + std::pow(a.y - b.y, 2.0));
}

double angle(const UnwrappedPose &state,
             UnwrappedPose reference,
             const bool flipOnBlue) {
  if(flipOnBlue) {
    if(GUI::Routines::selectedColor() == MatchColor::Blue) {
      reference.flip();
    }
  }
  const double dx{reference.x - state.x};
  const double dy{reference.y - state.y};
  double dh{M_PI / 2 - std::atan2(dy, dx)};
  return constrainPI(dh);
}

std::string toString(const UnwrappedPose &pose) {
  return "(" + std::to_string(pose.x) + " m, " + std::to_string(pose.y) +
         " m, " + std::to_string(pose.h) + " rad)";
}

Pose::Pose(const meter_t iX,
           const meter_t iY,
           const radian_t iH,
           const meters_per_second_t iV,
           const meters_per_second_squared_t iA,
           const radians_per_second_t iOmega,
           const radians_per_second_squared_t iAlpha,
           const second_t iT) :
    x{iX}, y{iY}, h{iH}, v{iV}, a{iA}, omega{iOmega}, alpha{iAlpha}, t{iT} {}

Pose::Pose(const UnwrappedPose &unwrappedPose) :
    x{meter_t{unwrappedPose.x}},
    y{meter_t{unwrappedPose.y}},
    h{radian_t{unwrappedPose.h}},
    v{meters_per_second_t{unwrappedPose.v}},
    a{meters_per_second_squared_t{unwrappedPose.a}},
    omega{radians_per_second_t{unwrappedPose.omega}},
    alpha{radians_per_second_squared_t{unwrappedPose.alpha}},
    t{second_t{unwrappedPose.t}} {}

bool Pose::operator==(const Pose &rhs) const {
  return x == rhs.x && y == rhs.y && h == rhs.h;
}

bool Pose::operator!=(const Pose &rhs) const {
  return !(*this == rhs);
}

Pose Pose::operator+(const Pose &rhs) const {
  return {x + rhs.x, y + rhs.y};
}

Pose Pose::operator-(const Pose &rhs) const {
  return {x - rhs.x, y - rhs.y};
}

void Pose::flip() {
  x *= -1;
  h *= -1;
}

Pose operator*(const double lhs, const Pose &rhs) {
  return {lhs * rhs.x, lhs * rhs.y};
}

Pose operator*(const Pose &lhs, const double rhs) {
  return {lhs.x * rhs, lhs.y * rhs};
}

tile_t distance(const Pose &a, const Pose &b) {
  return sqrt(pow<2>(a.x - b.x) + pow<2>(a.y - b.y));
}

degree_t angle(const Pose &state, Pose reference, const bool flipOnBlue) {
  if(flipOnBlue) {
    if(GUI::Routines::selectedColor() == MatchColor::Blue) {
      reference.flip();
    }
  }
  const inch_t dx{reference.x - state.x};
  const inch_t dy{reference.y - state.y};
  degree_t dh{90_deg - atan2(dy, dx)};
  return constrain180(dh);
}

std::string toString(const Pose &pose) {
  return "(" + to_string(pose.x) + ", " + to_string(pose.y) + ", " +
         to_string(pose.h) + ")";
}
} // namespace atum