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
#include "../systems/drive.hpp"
#include "../utility/acceptable.hpp"
#include "path.hpp"
#include "profileFollower.hpp"

namespace atum {
/**
 * @brief Encapsulates the logic behind path following. Can use RAMSETE for
 * feedback control or motion-profiled movements.
 *
 */
class PathFollower {
  public:
  /**
   * @brief The commands that the path follower executes.
   *
   */
  struct Command {
    /**
     * @brief Constructs a new Command object. A path is generated from the
     * current pose of the drive to the given target pose.
     *
     * @param iTarget
     * @param iReversed
     * @param iParams
     * @param iAcceptable
     */
    Command(const Pose &iTarget,
            bool iReversed = false,
            std::optional<Path::Parameters> iParams = {},
            std::optional<AcceptableDistance> iAcceptable = {});
    Pose target;
    bool reversed{false};
    std::optional<Path::Parameters> params;
    std::optional<AcceptableDistance> acceptable;
    // By default, time out after path takes 10% longer than expected.
    double timeoutScaling{1.1};
  };

  /**
   * @brief The parameters for the feedback component of path following. Can
   * enable/disabled RAMSETE and change its beta and lambda values.
   *
   */
  struct FeedbackParameters {
    bool useRAMSETE{true};
    double beta{2.0};
    double lambda{0.7};
  };

  /**
   * @brief Constructs a new PathFollower object.
   *
   * @param iDrive
   * @param iDefaultAcceptable
   * @param iLeft
   * @param iRight
   * @param iKA
   * @param iFeedbackParams
   * @param loggerLevel
   */
  PathFollower(Drive *iDrive,
               const AcceptableDistance &iDefaultAcceptable,
               std::unique_ptr<Controller> iLeft,
               std::unique_ptr<Controller> iRight,
               const AccelerationConstants &iKA,
               const FeedbackParameters &iFeedbackParams =
                   FeedbackParameters{true, 2.0, 0.7},
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

  /**
   * @brief Interrupts the current command(s).
   *
   */
  void interrupt();

  /**
   * @brief Sets whether the commands should be flipped across the x-axis (if
   * the color is changed).
   *
   * @param iFlipped
   */
  void setFlipped(const bool iFlipped);

  private:
  /**
   * @brief Follows a single command.
   *
   * @param cmd
   */
  void follow(Command cmd);

  /**
   * @brief Gets the current reference for the left and right sides of the drive
   * train.
   *
   * @param state
   * @param target
   * @param reversed
   * @return std::pair<double, double>
   */
  std::pair<double, double> getLRReference(const UnwrappedPose &state,
                                           const UnwrappedPose &target,
                                           const bool reversed);

  /**
   * @brief Gets the current reference velocity and angular velocity (first and
   * second members of the returned pair, respectively).
   *
   * @param state
   * @param target
   * @return std::pair<double, double>
   */
  std::pair<double, double> getReference(const UnwrappedPose &state,
                                         const UnwrappedPose &target);

  /**
   * @brief Gets the error between the state and the target.
   *
   * @param state
   * @param target
   * @return UnwrappedPose
   */
  UnwrappedPose getError(const UnwrappedPose &state,
                         const UnwrappedPose &target);

  /**
   * @brief Converts a given velocity and angular velocity to left and right
   * side RPMs.
   *
   * @param v
   * @param omega
   * @return std::pair<double, double>
   */
  std::pair<double, double> toRPM(const double v, const double omega);

  /**
   * @brief Gets the feedforward for acceleration based on the acceleration of
   * the target and if the path is reversed.
   *
   * @param a
   * @param reversed
   * @return double
   */
  double getAccelFeedforward(const double a, const bool reversed);

  /**
   * @brief Converts a lateral and angular quantity into corresponding
   * quantities on the left and right side of the drive.
   *
   * @param lateral
   * @param angular
   * @return std::pair<double, double>
   */
  std::pair<double, double> toLR(const double lateral, const double angular);

  /**
   * @brief Prepare the graph screen.
   *
   */
  void prepareGraph();

  /**
   * @brief Graphs a point.
   *
   * @param stateVL
   * @param refVL
   * @param stateVR
   * @param refVR
   */
  void graphPoints(const double stateVL,
                   const double refVL,
                   const double stateVR,
                   const double refVR);

  Drive *drive;
  AcceptableDistance defaultAcceptable;
  std::unique_ptr<Controller> left;
  std::unique_ptr<Controller> right;
  AccelerationConstants kA;
  FeedbackParameters feedbackParams;
  Logger logger;
  bool interrupted{false};
  bool flipped{false};
};
} // namespace atum