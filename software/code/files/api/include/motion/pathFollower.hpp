/**
 * @file pathFollower.hpp
 * @brief Includes the PathFollower class.
 * @date 2025-01-09
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "../controllers/controller.hpp"
#include "../controllers/slewRate.hpp"
#include "../systems/drive.hpp"
#include "../utility/acceptable.hpp"
#include "movement.hpp"
#include "path.hpp"
#include "profileFollower.hpp"


namespace atum {
/**
 * @brief Encapsulates the logic behind path following, using pure pursuit.
 *
 */
class PathFollower : public Movement {
  public:
  /**
   * @brief The commands that the path follower executes.
   *
   */
  struct Command {
    /**
     * @brief Constructs a new Command object. A path is generated from the
     * given starting pose to the given target pose. This is the generally
     * preferred method.
     *
     * @param iAcceptable
     * @param iStart
     * @param iTarget
     * @param iReversed
     * @param iParams
     */
    Command(std::optional<AcceptableDistance> iAcceptable,
            const Pose &iStart,
            const Pose &iTarget,
            const bool iReversed = false,
            std::optional<Path::Parameters> iParams = {});

    /**
     * @brief Constructs a new Command object. A path is generated from the
     * current pose of the drive to the given target pose.
     *
     * @param iAcceptable
     * @param iTarget
     * @param iReversed
     * @param iParams
     */
    Command(std::optional<AcceptableDistance> iAcceptable,
            const Pose &iTarget,
            const bool iReversed = false,
            std::optional<Path::Parameters> iParams = {});

    std::optional<AcceptableDistance> acceptable;
    std::optional<Pose> start{};
    Pose target;
    bool reversed{false};
    std::optional<Path::Parameters> params;
  };

  /**
   * @brief Constructs a new PathFollower object.
   *
   * @param iDrive
   * @param iDefaultAcceptable
   * @param iForward
   * @param iTurn
   * @param iKA
   * @param loggerLevel
   */
  PathFollower(Drive *iDrive,
               const AcceptableDistance &iDefaultAcceptable,
               std::unique_ptr<Controller> iForward,
               std::unique_ptr<Controller> iTurn,
               const AccelerationConstants &iKA,
               const meter_t iLookaheadDistance = 1_ft,
               const Logger::Level loggerLevel = Logger::Level::Info);

  /**
   * @brief Follows a list of given commands. Can provide a name for debugging
   * purposes.
   *
   * @param commands
   * @param name
   */
  void follow(const std::vector<Command> &commands,
              const std::string &name = "");

  private:
  /**
   * @brief Follows a single command.
   *
   * @param cmd
   */
  void follow(Command cmd);

  /**
   * @brief Resets the controllers and internal state of the path follower
   * before following another path.
   *
   * Mutable reference is to allow the flipping and reversing of the target if
   * necessary.
   *
   * @param cmd
   */
  void reset(PathFollower::Command &cmd);

  /**
   * @brief Gets the references velocity and heading.
   *
   * @param state
   * @return std::pair<double, double>
   */
  std::pair<double, double> getVHReference(const Pose &state);

  /**
   * @brief Gets the feedforward for acceleration based on the change in the
   * reference.
   *
   * @param refVL
   * @return double
   */
  double getAccelFeedforward(const double refV, const bool reversed);

  /**
   * @brief Updates the current index of the lookahead and returns the pose at
   * that index.
   *
   * @param state
   * @return Pose
   */
  Pose getLookahead(const Pose &state);

  /**
   * @brief Updates the current index of the closest pose on the path and
   * returns the pose at that index.
   *
   * @param state
   * @return Pose
   */
  Pose getClosest(const Pose &state);

  /**
   * @brief Returns the squared distance between the points to avoid calling
   * sqrt.
   *
   * @param p0
   * @param p1
   * @return double
   */
  double proximity(const UnwrappedPose &p0, const UnwrappedPose &p1) const;

  /**
   * @brief Prepare the graph screen.
   *
   */
  void prepareGraph();

  /**
   * @brief Graphs a point.
   *
   * @param stateV
   * @param refV
   */
  void graphPoints(const double stateV, const double refV);

  Drive *drive;
  AcceptableDistance defaultAcceptable;
  std::unique_ptr<Controller> forward;
  std::unique_ptr<Controller> turn;
  AccelerationConstants kA;
  const double lookaheadDistance; // In meters.
  Logger logger;
  std::unique_ptr<Path> path;
  std::unique_ptr<SlewRate> accelLimiter;
  Pose closest;
  int closestIndex{0};
  Pose lookahead;
  int lookaheadIndex{0};
  double prevRefV{0.0};
};
} // namespace atum