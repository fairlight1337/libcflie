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

using namespace std;


enum Controller {
  CTRL_NONE = 0,
  CTRL_P = 1
};

/*! \brief Crazyflie Nano convenience controller class

 * The class containing the mechanisms for starting sensor readings,
 * ordering set point setting, selecting and running controllers and
 * calculating information based on the current sensor readings. */
class CCrazyflie {
 private:
  // Variables
  /*! Internal pointer to the initialized CCrazyRadio radio interface
      instance. */
  CCrazyRadio *m_crRadio;
  int m_nLastRequestedVariableIndex;
  /*! The current thrust to send as a set point to the copter. */
  int m_nThrust;
  /*! The current roll to send as a set point to the copter. */
  float m_fRoll;
  /*! The current pitch to send as a set point to the copter. */
  float m_fPitch;
  /*! The current yaw to send as a set point to the copter. */
  float m_fYaw;
  /*! The currently selected controller. */
  CController *m_ctrlController;
  struct DSPose m_dspCurrentPose;
  struct DSTwist m_dstCurrentTwist;
  struct DSControlSetPoint m_cspDesired;
  enum Controller m_enumCtrl;
  /*! The point in time (in seconds) at which the last set point was
      sent to the copter. Serves the purpose of calculating controller
      related variables and integrals. */
  double m_dSecondsLast;
  
  // Control related parameters
  /*! Maximum absolute value for the roll that will be sent to the
      copter. */
  float m_fMaxAbsRoll;
  /*! Maximum absolute value for the pitch that will be sent to the
      copter. */
  float m_fMaxAbsPitch;
  /*! Maximum absolute value for the yaw that will be sent to the
      copter. */
  float m_fMaxYaw;
  /*! Maximum thrust that will be sent to the copter. */
  int m_nMaxThrust;
  /*! Minimum thrust that will be sent to the copter. */
  int m_nMinThrust;
  /*! Whether or not the controllers ignore controlling the yaw of the
      copter. Since for navigation, it is not important in which
      direction the copter points, it might be switched off to give
      the controllers a larger degree of freedom. This setting is
      forwarded to the currently active (or thereafter activated)
      controller. */
  bool m_bControllerIgnoresYaw;
  /*! Threshold value which denotes the maximum distance the copter
      position might have from the set desired position for marking it
      as arrived. */
  float m_fArrivalThreshold;

  // Functions
  /*! Resetting the internal state (pose and twist) of the copter
      controller(s). This is usually not called outside of the class
      and has mainly initialization purpose. */
  void resetState();
  double currentTime();
  
  struct DSVelocityControlSignal identityControlSignal();
  void applyControllerResult(double dElapsedTime);
  void calculatePoseIntegral(double dElapsedTime);
  void calculateCartesianVelocity();

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
  
  /*! \brief Set thrust control set point
    
    The thrust value that will be sent to the internal copter
    controller as a set point.
    
    \param nThrust The thrust value to send (> 10000) */
  void setThrust(int nThrust);
  int thrust();
  
  /*! \brief Set roll control set point
    
    The roll value that will be sent to the internal copter
    controller as a set point.
    
    \param fRoll The roll value to send */
  void setRoll(float fRoll);
  float roll();

  /*! \brief Set pitch control set point
    
    The pitch value that will be sent to the internal copter
    controller as a set point.
    
    \param fPitch The pitch value to send */
  void setPitch(float fPitch);
  float pitch();

  /*! \brief Set yaw control set point
    
    The yaw value that will be sent to the internal copter
    controller as a set point.
    
    \param fYaw The yaw value to send */
  void setYaw(float fYaw);
  float yaw();
  
  bool cycle();
  bool copterInRange();
  
  void disableController();
  void setPController(float fPGain);
  
  void setDesiredSetPoint(struct DSControlSetPoint cspDesired);
  
  double distanceBetweenPositions(struct DSVector dsvPosition1, struct DSVector dsvPosition2);
  double distanceToPosition(struct DSVector dsvPosition);
  double distanceToDesiredPosition();
  void goToRelativePosition(struct DSVector dsvRelative);
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
  
  void setArrivalThreshold(float fArrivalThreshold);
  float arrivalThreshold();
  
  void setControllerIgnoresYaw(bool bControllerIgnoresYaw);
  bool controllerIgnoresYaw();
};


#endif /* __C_CRAZYFLIE_H__ */
