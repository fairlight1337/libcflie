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
#include "CTOC.h"

using namespace std;


enum State {
  STATE_ZERO = 0,
  STATE_READ_PARAMETERS_TOC = 1,
  STATE_READ_LOGS_TOC = 2,
  STATE_START_LOGGING = 3,
  STATE_ZERO_MEASUREMENTS = 4,
  STATE_NORMAL_OPERATION = 5
};

/*! \brief Crazyflie Nano convenience controller class

  The class containing the mechanisms for starting sensor readings,
  ordering set point setting, selecting and running controllers and
  calculating information based on the current sensor readings. */
class CCrazyflie {
 private:
  // Variables
  int m_nAckMissTolerance;
  int m_nAckMissCounter;
  /*! \brief Internal pointer to the initialized CCrazyRadio radio
      interface instance. */
  CCrazyRadio *m_crRadio;
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
  /*! \brief The current desired control set point (position/yaw to
      reach) */
  
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
  double m_dSendSetpointPeriod;
  double m_dSetpointLastSent;
  bool m_bSendsSetpoints;
  CTOC *m_tocParameters;
  CTOC *m_tocLogs;
  enum State m_enumState;
  
  // Functions
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

  void disableLogging();
  
  void enableStabilizerLogging();
  void enableGyroscopeLogging();
  void enableAccelerometerLogging();

  void disableStabilizerLogging();
  void disableGyroscopeLogging();
  void disableAccelerometerLogging();
  
  void enableBatteryLogging();
  void disableBatteryLogging();
  
  bool startLogging();
  bool stopLogging();

  void enableMagnetometerLogging();
  void disableMagnetometerLogging();

  void enableAltimeterLogging();
  void disableAltimeterLogging();

  double currentTime();

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
  /*! \brief Returns the current thrust
    
    \return The current thrust value as reported by the copter */
  int thrust();
  
  /*! \brief Set the roll control set point
    
    The roll value that will be sent to the internal copter
    controller as a set point.
    
    \param fRoll The roll value to send */
  void setRoll(float fRoll);
  /*! \brief Returns the current roll
    
    Roll values are in degree, ranging from -180.0deg to 180.0deg.
    
    \return The current roll value as reported by the copter */
  float roll();
  
  /*! \brief Set the pitch control set point
    
    The pitch value that will be sent to the internal copter
    controller as a set point.
    
    \param fPitch The pitch value to send */
  void setPitch(float fPitch);
  /*! \brief Returns the current pitch
    
    Pitch values are in degree, ranging from -180.0deg to 180.0deg.

    \return The current pitch value as reported by the copter */
  float pitch();

  /*! \brief Set the yaw control set point
    
    The yaw value that will be sent to the internal copter
    controller as a set point.
    
    \param fYaw The yaw value to send */
  void setYaw(float fYaw);
  /*! \brief Returns the current yaw

    Yaw values are in degree, ranging from -180.0deg to 180.0deg.
    
    \return The current yaw value as reported by the copter */
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
  
  /*! \brief Whether or not the copter was initialized successfully.
    
    \returns Boolean value denoting the initialization status of the
    copter communication. */
  bool isInitialized();
  
  /*! \brief Set whether setpoints are currently sent while cycle()
    
    While performing the cycle() function, the currently set setpoint
    is sent to the copter regularly. If this is not the case, dummy
    packets are sent. Using this mechanism, you can effectively switch
    off sending new commands to the copter.
    
    Default value: `false`
    
    \param bSendSetpoints When set to `true`, the current setpoint is
    sent while cycle(). Otherwise, not. */
  void setSendSetpoints(bool bSendSetpoints);
  
  /*! \brief Whether or not setpoints are currently sent to the copter
    
    \return Boolean value denoting whether or not the current setpoint
    is sent to the copter while performing cycle(). */
  bool sendsSetpoints();
  
  /*! \brief Read back a sensor value you subscribed to
    
    Possible sensor values might be:
    * stabilizer.yaw
    * stabilizer.roll
    * stabilizer.pitch
    * pm.vbat
    
    The possible key names strongly depend on your firmware. If you
    don't know what to do with this, just use the convience functions
    like roll(), pitch(), yaw(), and batteryLevel().
    
    \return Double value denoting the current value of the requested
    log variable. */
  double sensorDoubleValue(string strName);
  
  /*! \brief Report the current battery level
    
    \return Double value denoting the battery level as reported by the
    copter. */
  double batteryLevel();

  float accX();
  float accY();
  float accZ();
  float accZW();
  float asl();
  float aslLong();
  float temperature();
  float pressure();
  float gyroX();
  float gyroY();
  float gyroZ();
  float batteryState();
  float magX();
  float magY();
  float magZ();

};


#endif /* __C_CRAZYFLIE_H__ */
