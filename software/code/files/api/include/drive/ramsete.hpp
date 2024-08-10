#pragma once

#include "../controllers/controller.hpp"
#include "../drive/drive.hpp"
#include "../motion/trajectory.hpp"
#include "pointAt.hpp"

namespace atum {
class RAMSETE {
  public:
  using Constraints = LateralMotionProfile::MotionConstraints;

  RAMSETE(Drive *iDrive,
          std::shared_ptr<PointAt> iPointAt,
          std::unique_ptr<Controller> iLeft,
          std::unique_ptr<Controller> iRight,
          const Constraints &iDefaultConstraints,
          const double iBeta = 2.0,
          const double iLambda = 0.7);

  void go(const std::string &trajName,
          const bool updateTraj,
          std::vector<Position> waypoints,
          const bool reversed = false,
          const std::optional<Constraints> &specialConstraints = {},
          const double curvature = 1.0);

  private:
  void performRAMSETE(RawPosition state,
                      const RawPosition &target,
                      const bool reversed);

  RawPosition getError(const RawPosition &state, const RawPosition &target);

  Drive *drive;
  std::shared_ptr<PointAt> pointAt;
  std::unique_ptr<Controller> left;
  std::unique_ptr<Controller> right;
  Constraints defaultConstraints;
  const double beta;
  const double lambda;
};
} // namespace atum