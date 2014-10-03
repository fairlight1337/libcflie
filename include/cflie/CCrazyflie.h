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


/* \author Jan Winkler */


#ifndef __C_CRAZYFLIE_H__
#define __C_CRAZYFLIE_H__


#include <iostream>
#include <sstream>
#include <math.h>

#include "CCrazyRadio.h"
#include "CTOC.h"


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
  uint16_t m_nThrust;
  /*! \brief The current roll to send as a set point to the copter. */
  float m_fRoll;
  /*! \brief The current pitch to send as a set point to the
      copter. */
  float m_fPitch;
  /*! \brief The current yaw to send as a set point to the copter. */
  float m_fYaw;

  // Control related parameters
  bool m_bSendsSetpoints;
  CTOC m_tocParameters;
  CTOC m_tocLogs;
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
  bool sendSetpoint(float fRoll, float fPitch, float fYaw, uint16_t sThrust);

  void disableLogging();

  void enableStabilizerLogging();
  void disableStabilizerLogging() {
    m_tocLogs.unregisterLoggingBlock("stabilizer");
  }

  void enableGyroscopeLogging();
  void disableGyroscopeLogging() {
    m_tocLogs.unregisterLoggingBlock("gyroscope");
  }

  void enableAccelerometerLogging();
  void disableAccelerometerLogging() {
    m_tocLogs.unregisterLoggingBlock("accelerometer");
  }

  void enableBatteryLogging();
  void disableBatteryLogging() {
    m_tocLogs.unregisterLoggingBlock("battery");
  }

  bool startLogging();
  bool stopLogging();

  void enableMagnetometerLogging();
  void disableMagnetometerLogging() {
    m_tocLogs.unregisterLoggingBlock("magnetometer");
  }

  void enableBarometerLogging();
  void disableBarometerLogging() {
    m_tocLogs.unregisterLoggingBlock("barometer");
  }

  float logFloat(const std::string& val) const {
    float f;
    if(getLogValue(val, f))
      return -1;
    return f;
  }
  uint16_t logU16(const std::string& val) const {
    uint16_t u16;
    if(getLogValue(val, u16))
      return 0;
    return u16;
  }
  uint16_t logI8(const std::string& val) const {
    int8_t i8;
    if(getLogValue(val, i8))
      return -1;
    return i8;
  }

 public:
  /*! \brief Constructor for the copter convenience class

    Constructor for the CCrazyflie class, taking a CCrazyRadio radio
    interface instance as a parameter.
  
    \param crRadio Initialized (and started) instance of the
    CCrazyRadio class, denoting the USB dongle to communicate
    with. */
  CCrazyflie(CCrazyRadio *crRadio)
    : m_crRadio(crRadio)
    , m_nThrust(0)
    , m_fRoll(0)
    , m_fPitch(0)
    , m_fYaw(0)
    , m_bSendsSetpoints(false)
    , m_tocParameters(crRadio, CCRTPPacket::PortParam)
    , m_tocLogs(crRadio, CCRTPPacket::PortLogging)
    , m_enumState(STATE_ZERO)
  {}

  /*! \brief Destructor for the copter convenience class
    
    Destructor, deleting all internal variables (except for the
    CCrazyRadio radio instance given in the constructor). */
  ~CCrazyflie();
  
  /*! \brief Set the thrust control set point
    
    The thrust value that will be sent to the internal copter
    controller as a set point.
    
    \param nThrust The thrust value to send (> 10000) */
  void setThrust(uint16_t nThrust) {
    m_nThrust = nThrust;
  }
  /*! \brief Returns the current thrust
    
    \return The current thrust value as reported by the copter */
  uint16_t thrust(void) const {
    return logU16("stabilizer.thrust");
  }
  /*! \brief Set the roll control set point
    
    The roll value that will be sent to the internal copter
    controller as a set point.
    
    \param fRoll The roll value to send */
  void setRoll(float fRoll) {
    m_fRoll = fRoll;
  }

  /*! \brief Returns the current roll
    
    Roll values are in degree, ranging from -180.0deg to 180.0deg.
    
    \return The current roll value as reported by the copter */
  float roll(void) const {
    return logFloat("stabilizer.roll");
  }
  /*! \brief Set the pitch control set point
    
    The pitch value that will be sent to the internal copter
    controller as a set point.
    
    \param fPitch The pitch value to send */
  void setPitch(float fPitch) {
    m_fPitch = fPitch;
  }
  /*! \brief Returns the current pitch
    
    Pitch values are in degree, ranging from -180.0deg to 180.0deg.

    \return The current pitch value as reported by the copter */
  float pitch(void) const {
    return logFloat("stabilizer.pitch");
  }
  /*! \brief Set the yaw control set point
    
    The yaw value that will be sent to the internal copter
    controller as a set point.
    
    \param fYaw The yaw value to send */
  void setYaw(float fYaw) {
    m_fYaw = fYaw;
  }
  /*! \brief Returns the current yaw

    Yaw values are in degree, ranging from -180.0deg to 180.0deg.
    
    \return The current yaw value as reported by the copter */
  float yaw(void) const {
    return logFloat("stabilizer.yaw");
  }
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
  bool copterInRange() {
    return m_nAckMissCounter < m_nAckMissTolerance;
  }
  /*! \brief Whether or not the copter was initialized successfully.
    
    \returns Boolean value denoting the initialization status of the
    copter communication. */
  bool isInitialized(void) const {
    return m_enumState == STATE_NORMAL_OPERATION;
  }
  /*! \brief Set whether setpoints are currently sent while cycle()
    
    While performing the cycle() function, the currently set setpoint
    is sent to the copter regularly. If this is not the case, dummy
    packets are sent. Using this mechanism, you can effectively switch
    off sending new commands to the copter.
    
    Default value: `false`
    
    \param bSendSetpoints When set to `true`, the current setpoint is
    sent while cycle(). Otherwise, not. */
  void setSendSetpoints(bool bSendSetpoints) {
    m_bSendsSetpoints = bSendSetpoints;
  }
  /*! \brief Whether or not setpoints are currently sent to the copter
    
    \return Boolean value denoting whether or not the current setpoint
    is sent to the copter while performing cycle(). */
  bool sendsSetpoints(void) const {
    return m_bSendsSetpoints;
  }

  // Get log values
  template <typename t>
  int getLogValue(const std::string& strName, t& value) const {
    return m_tocLogs.getLogValue(strName, value);
  }

  /*! \brief Report the current battery level
    
    \return Float value denoting the battery level as reported by the
    copter. */
  float batteryLevel(void) const {
    return logFloat("pm.vbat");
  }

  float accX(void) const {
    return logFloat("acc.x");
  }
  float accY(void) const {
    return logFloat("acc.y");
  }
  float accZ(void) const {
    return logFloat("acc.z");
  }
  float accZW(void) const {
    return logFloat("acc.zw");
  }
  float asl(void) const {;
    return logFloat("baro.asl");
  }
  float aslRaw(void) const {
    return logFloat("baro.aslRaw");
  }
  float aslLong(void) const {
    return logFloat("baro.aslLong");
  }
  float temperature(void) const {
    return logFloat("baro.temp");
  }
  float pressure(void) const {
    return logFloat("baro.pressure");
  }
  float gyroX(void) const {
    return logFloat("gyro.x");
  }
  float gyroY(void) const {
    return logFloat("gyro.y");
  }
  float gyroZ(void) const {
    return logFloat("gyro.z");
  }
  int8_t batteryState(void) const {
    return logI8("pm.state");
  }
  float magX(void) const {
    return logFloat("mag.x");
  }
  float magY(void) const {
    return logFloat("mag.y");
  }
  float magZ(void) const {
    return logFloat("mag.z");
  }
  // Set parameter values
  template <typename t>
  int setParameterValue(const std::string& strName, t value) {
    return m_tocParameters.setParameterValue(strName, value);
  }
  // Get parameter values
  template <typename t>
  int getParameterValue(const std::string& strName, t& value) const {
    return m_tocParameters.getParameterValue(strName, value);
  }
  int requestParameterValue(const std::string& strName) {
    return m_tocParameters.requestParameterValue(strName);
  }
};


#endif /* __C_CRAZYFLIE_H__ */
