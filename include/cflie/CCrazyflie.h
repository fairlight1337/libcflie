// Copyright (c) 2013, Jan Winkler <winkler@cs.uni-bremen.de>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Universität Bremen nor the names of its
//       contributors may be used to endorse or promote products derived from
//       this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef __C_CRAZYFLIE_H__
#define __C_CRAZYFLIE_H__


#include <iostream>
#include <sstream>
#include <math.h>

#include "CCrazyRadio.h"
#include "CController.h"
#include "CPController.h"
#include "CTOC.h"

using namespace std;


enum State {
  STATE_ZERO = 0,
  STATE_READ_PARAMETERS_TOC = 1,
  STATE_READ_LOGS_TOC = 2,
  STATE_START_LOGGING = 3,
  STATE_NORMAL_OPERATION = 4
};

/*! \brief Enumeration holding the possible (i.e. implemented)
    controller types */
enum Controller {
  CTRL_NONE = 0,
  CTRL_P = 1
};

/*! \brief Crazyflie Nano convenience controller class

  The class containing the mechanisms for starting sensor readings,
  ordering set point setting, selecting and running controllers and
  calculating information based on the current sensor readings. */
class CCrazyflie {
 private:
  // Variables
  /*! \brief Internal pointer to the initialized CCrazyRadio radio
      interface instance. */
  CCrazyRadio *m_crRadio;
  /*! \brief Internal index for requesting copter TOC variables */
  int m_nLastRequestedVariableIndex;
  /*! \brief The current thrust to send as a set point to the
      copter. */
  int m_nThrust;
  /*! \brief The current roll to send as a set point to the copter. */
  float m_fRoll;
  /*! \brief The current pitch to send as a set point to the
      copter. */
  float m_fPitch;
  /*! \brief The current yaw to send as a set point to the copter. */
  float m_fYaw;
  /*! \brief The currently selected controller. */
  CController *m_ctrlController;
  /*! \brief Holding the current copter pose as read from the sensors
      and calculated (integrated) over time */
  struct DSPose m_dspCurrentPose;
  /*! \brief Holding the current copter twist as reported by the
      sensor readings */
  struct DSTwist m_dstCurrentTwist;
  /*! \brief The current desired control set point (position/yaw to
      reach) */
  struct DSControlSetPoint m_cspDesired;
  enum Controller m_enumCtrl;
  /*! \brief Time of last cycle start
    
    The point in time (in seconds) at which the last set point was
    sent to the copter. Serves the purpose of calculating controller
    related variables and integrals. */
  double m_dSecondsLast;
  
  // Control related parameters
  /*! \brief Maximum absolute value for the roll that will be sent to
      the copter. */
  float m_fMaxAbsRoll;
  /*! \brief Maximum absolute value for the pitch that will be sent to
      the copter. */
  float m_fMaxAbsPitch;
  /*! \brief Maximum absolute value for the yaw that will be sent to
      the copter. */
  float m_fMaxYaw;
  /*! \brief Maximum thrust that will be sent to the copter. */
  int m_nMaxThrust;
  /*! \brief Minimum thrust that will be sent to the copter. */
  int m_nMinThrust;
  bool m_bSendsSetpoints;
  /*! \brief Whether or not controllers ignore yaw control
    
    Whether or not the controllers ignore controlling the yaw of the
    copter. Since for navigation, it is not important in which
    direction the copter points, it might be switched off to give the
    controllers a larger degree of freedom. This setting is forwarded
    to the currently active (or thereafter activated) controller. */
  bool m_bControllerIgnoresYaw;
  /*! \brief Threshold value for determining arrival status
    
    Threshold value which denotes the maximum distance the copter
    position might have from the set desired position for marking it
    as arrived. */
  float m_fArrivalThreshold;
  CTOC *m_tocParameters;
  CTOC *m_tocLogs;
  enum State m_enumState;

  // Functions
  /*! \brief Reset the internal state
    
    Resetting the internal state (pose and twist) of the copter
    controller(s). This is usually not called outside of the class and
    has mainly initialization purpose. */
  void resetState();
  /*! \brief Reports the current time in fractions of seconds
    
    Calculates and returns the current system time in (fractions of)
    seconds. This time is mainly used for integrating over time and
    control signal application purposes.
    
    \return Value denoting the current time since epoch (1970/01/01
    00:00:00) in fractions of seconds */
  double currentTime();
  
  struct DSVelocityControlSignal identityControlSignal();
  void applyControllerResult(double dElapsedTime);
  void calculatePoseIntegral(double dElapsedTime);
  void calculateCartesianVelocity();
  bool readTOCParameters();
  bool readTOCLogs();
  
  /*! \brief Send a set point to the copter controller

    Send the set point for the internal copter controllers. The
    copter will then try to achieve the given roll, pitch, yaw and
    thrust. These values can be set manually but are managed by the
    herein available controller(s) if one is switched on to reach
    desired positions.
    
    \param fRoll The desired roll value.
    \param fPitch The desired pitch value.
    \param fYaw The desired yaw value.
    \param sThrust The desired thrust value.
    \return Boolean value denoting whether or not the command could be sent successfully. */
  bool sendSetpoint(float fRoll, float fPitch, float fYaw, short sThrust);

  void updateTOC();
  void populateTOCElement(int nIndex);
  void populateNextTOCElement();

  void populateLOGElement(int nIndex);
  void populateNextLOGElement();
  
  void updateLogTOC();
    
 public:
  /*! \brief Constructor for the copter convenience class

    Constructor for the CCrazyflie class, taking a CCrazyRadio radio
    interface instance as a parameter.
  
    \param crRadio Initialized (and started) instance of the
    CCrazyRadio class, denoting the USB dongle to communicate
    with. */
  CCrazyflie(CCrazyRadio *crRadio);
  /*! \brief Destructor for the copter convenience class
    
    Destructor, deleting all internal variables (except for the
    CCrazyRadio radio instance given in the constructor). */
  ~CCrazyflie();
  
  /*! \brief Set the thrust control set point
    
    The thrust value that will be sent to the internal copter
    controller as a set point.
    
    \param nThrust The thrust value to send (> 10000) */
  void setThrust(int nThrust);
  /*! \brief Returns the current thrust control set point
    
    \return The current thrust control set point as given to the
    current controller */
  int thrust();
  
  /*! \brief Set the roll control set point
    
    The roll value that will be sent to the internal copter
    controller as a set point.
    
    \param fRoll The roll value to send */
  void setRoll(float fRoll);
  /*! \brief Returns the current roll control set point
    
    \return The current roll control set point as given to the current
    controller */
  float roll();
  
  /*! \brief Set the pitch control set point
    
    The pitch value that will be sent to the internal copter
    controller as a set point.
    
    \param fPitch The pitch value to send */
  void setPitch(float fPitch);
  /*! \brief Returns the current pitch control set point
    
    \return The current pitch control set point as given to the
    current controller */
  float pitch();

  /*! \brief Set the yaw control set point
    
    The yaw value that will be sent to the internal copter
    controller as a set point.
    
    \param fYaw The yaw value to send */
  void setYaw(float fYaw);
  /*! \brief Returns the current yaw control set point
    
    \return The current yaw control set point as given to the current
    controller */
  float yaw();
  
  /*! \brief Manages internal calculation operations
    
    Should be called during every 'cycle' of the main program using
    this class. Things like sensor reading processing, integral
    calculation and controller signal application are performed
    here. This function also triggers communication with the
    copter. Not calling it for too long will cause a disconnect from
    the copter's radio.
    
    \return Returns a boolean value denoting the current status of the
    radio dongle. If it returns 'false', the dongle was most likely
    removed or somehow else disconnected from the host machine. If it
    returns 'true', the dongle connection works fine. */
  bool cycle();
  /*! \brief Signals whether the copter is in range or not
    
    Returns whether the radio connection to the copter is currently
    active.
    
    \return Returns 'true' is the copter is in range and radio
    communication works, and 'false' if the copter is either out of
    range or is switched off. */
  bool copterInRange();
  
  /*! \brief Switches off the current controller
    
    Resets copter control to 'CTRL_NONE' as specified in the
    Controller enum and effectively switches off control. */
  void disableController();
  /*! \brief Selects the P gain based controller as active controller
    
    \param fPGain Denotes the proportional P gain constant to use for
    the controller */
  void setPController(float fPGain);
  
  /*! \brief Set the desired set point, i.e. position and yaw
    
    Sets the desired controller set point for the currently selected
    controller. The set point will be set even if no controller is
    selected and tried to be achieved as soon as a controller is
    active.
    
    \param cspDesired The desired set point. Consists of an R^3
    position and a yaw value. */
  void setDesiredSetPoint(struct DSControlSetPoint cspDesired);
  
  /*! \brief Returns the distance between two physical positions
    
    Calculates the euclidean distance between the two R^3 positions
    given as dsvPosition1 and dsvPosition2.
    
    \return Returns a value denoting the distance between the two
    given positions, in meters. */
  double distanceBetweenPositions(struct DSVector dsvPosition1, struct DSVector dsvPosition2);
  /*! \brief Returns the distance between the current and the given
    physical position.
    
    \return Returns a value denoting the distance between the current
    and given position dsvPosition, in meters. */
  double distanceToPosition(struct DSVector dsvPosition);
  /*! \brief Returns the distance between the current and the desired
    physical position.
    
    \return Returns a value denoting the distance between the current
    and desired position, in meters. */
  double distanceToDesiredPosition();
  
  /*! \brief Sets the desired relative copter position
    
    Sets the correct controller set point to go to the relative R^3
    position given in dsvRelative, starting from an arbitrary current
    position.
    
    \param dsvRelative The relative R^3 position to go to (given in
    meters) */
  void goToRelativePosition(struct DSVector dsvRelative);
  /*! \brief Sets the desired absolute copter position
    
    Sets the correct controller set point to go to the absolute R^3
    position given in dsvAbsolute. The localization is purely based on
    the starting pose the copter was in when it was initialized. That
    place is denoted as origin (0, 0, 0) in R^3.
    
    Because of the nature of drifting integration errors when summing
    up velocity sensor readings and the connected uncertainty, this
    positioning method is bound to be imperfect. It can be used as an
    approximation if one does not need perfect localization though.
    
    \param dsvAbsolute The absolute R^3 position to go to (given in
    meters) */
  void goToAbsolutePosition(struct DSVector dsvAbsolute);
  
  /*! \brief Whether or not the copter has reached it's desired position.
    
    Taking an threshold value into account (which by default is set to
    0.05m), this function returns a boolean value showing whether the
    copter is in vicinity of the desired position set earlier. This
    value serves an informational purpose for client programs to see
    whether their control goal was reached yet.

    \return Boolean value denoting whether the control goal position was reached. */
  bool isAtDesiredPosition();
  /*! \brief Whether or not the copter has reached it's desired
      position, given a custom threshold value.
    
    Basically the same as isAtDesiredPosition() but gives the
    possibility of overriding the internal default threshold with a
    custom value.

    \param fThreshold Custom threshold value used for vicinity
    checking of the copter's control goal position.

    \return Boolean value denoting whether the control goal position
    was reached. */
  bool isAtDesiredPosition(float fThreshold);
  
  /*! \brief Sets the internal threshold value for arrival checking
    
    Sets the internal value of the threshold distance the copter must
    be within when checking whether it reached it's destination. This
    does not affect the control algorithm as they will continuously
    try to go as near as possible. Therefore, this value is purely
    used for user-side checking of destination vicinity.
  
    \param fArrivalThreshold The threshold value to set (in
    meters). */
  void setArrivalThreshold(float fArrivalThreshold);
  /*! \brief The current arrival threshold value
    
    \return Returns the current internal value for arrival vicinity
    checking. */
  float arrivalThreshold();
  
  /*! \brief Globally sets whether controllers ignore yaw control
    
    \param bControllerIgnoresYaw Boolean value that defines whether
    controllers ignore yaw control */
  void setControllerIgnoresYaw(bool bControllerIgnoresYaw);
  /*! \brief Signals whether or not the yaw control is ignored in
      controllers.
    
    \return Boolean value denoting whether the yaw control is
    currently ignored during control. */
  bool controllerIgnoresYaw();
  
  /*! \brief Reset the current pose to the origin pose
    
    Calls the private resetState function to re-localize the copter
    and use the current pose as the origin pose. Adding an extra
    function call here mainly serves splitting up functionalities
    between initialization and maintenance functions. */
  void relocalize();

  bool isInitialized();
  
  bool startLogging();
  void addHighSpeedLogging(string strName);
  void addLowSpeedLogging(string strName);
  
  void setSendSetpoints(bool bSendSetpoints);
  bool sendsSetpoints();
  
  double sensorDoubleValue(string strName);
};


#endif /* __C_CRAZYFLIE_H__ */
