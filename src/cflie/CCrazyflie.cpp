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

#include "cflie/CCrazyflie.h"


CCrazyflie::CCrazyflie(CCrazyRadio *crRadio) {
  m_crRadio = crRadio;
  
  // Review these values
  m_fMaxAbsRoll = 45.0f;
  m_fMaxAbsPitch = m_fMaxAbsRoll;
  m_fMaxYaw = 2 * M_PI;
  m_nMaxThrust = 60000;
  m_nMinThrust = 0;//15000;

  m_fRoll = 0;
  m_fPitch = 0;
  m_fYaw = 0;
  m_nThrust = 0;
  
  m_bSendsSetpoints = false;
  
  m_tocParameters = new CTOC(m_crRadio, 2);
  m_tocLogs = new CTOC(m_crRadio, 5);
  
  m_enumState = STATE_ZERO;
  
  m_dSendSetpointPeriod = 0.01; // Seconds
  m_dSetpointLastSent = 0;
}

CCrazyflie::~CCrazyflie() {
  this->stopLogging();
}

bool CCrazyflie::readTOCParameters() {
  if(m_tocParameters->requestMetaData()) {
    if(m_tocParameters->requestItems()) {
      return true;
    }
  }
  
  return false;
}

bool CCrazyflie::readTOCLogs() {
  if(m_tocLogs->requestMetaData()) {
    if(m_tocLogs->requestItems()) {
      return true;
    }
  }
  
  return false;
}

bool CCrazyflie::sendSetpoint(float fRoll, float fPitch, float fYaw, short sThrust) {
  fPitch = -fPitch;
  
  int nSize = 3 * sizeof(float) + sizeof(short);
  char cBuffer[nSize];
  memcpy(&cBuffer[0 * sizeof(float)], &fRoll, sizeof(float));
  memcpy(&cBuffer[1 * sizeof(float)], &fPitch, sizeof(float));
  memcpy(&cBuffer[2 * sizeof(float)], &fYaw, sizeof(float));
  memcpy(&cBuffer[3 * sizeof(float)], &sThrust, sizeof(short));
  
  CCRTPPacket *crtpPacket = new CCRTPPacket(cBuffer, nSize, 3);
  CCRTPPacket *crtpReceived = m_crRadio->sendPacket(crtpPacket);
  
  delete crtpPacket;
  if(crtpReceived != NULL) {
    delete crtpReceived;
    return true;
  } else {
    return false;
  }
}





void CCrazyflie::setThrust(int nThrust) {
  m_nThrust = nThrust;
  
  if(m_nThrust < m_nMinThrust) {
    m_nThrust = m_nMinThrust;
  } else if(m_nThrust > m_nMaxThrust) {
    m_nThrust = m_nMaxThrust;
  }
}

int CCrazyflie::thrust() {
  return this->sensorDoubleValue("stabilizer.thrust");
}

bool CCrazyflie::cycle() {
  double dTimeNow = this->currentTime();
  
  switch(m_enumState) {
  case STATE_ZERO: {
    m_enumState = STATE_READ_PARAMETERS_TOC;
  } break;
    
  case STATE_READ_PARAMETERS_TOC: {
    if(this->readTOCParameters()) {
      m_enumState = STATE_READ_LOGS_TOC;
    }
  } break;
    
  case STATE_READ_LOGS_TOC: {
    if(this->readTOCLogs()) {
      m_enumState = STATE_START_LOGGING;
    }
  } break;
    
  case STATE_START_LOGGING: {
    if(this->startLogging()) {
      m_enumState = STATE_ZERO_MEASUREMENTS;
    }
  } break;
    
  case STATE_ZERO_MEASUREMENTS: {
    m_tocLogs->processPackets(m_crRadio->popLoggingPackets());
    
    // NOTE(winkler): Here, we can do measurement zero'ing. This is
    // not done at the moment, though. Reason: No readings to zero at
    // the moment. This might change when altitude becomes available.
    
    m_enumState = STATE_NORMAL_OPERATION;
  } break;
    
  case STATE_NORMAL_OPERATION: {
    // Shove over the sensor readings from the radio to the Logs TOC.
    m_tocLogs->processPackets(m_crRadio->popLoggingPackets());
    
    if(m_bSendsSetpoints) {
      // Check if it's time to send the setpoint
      if(dTimeNow - m_dSetpointLastSent > m_dSendSetpointPeriod) {
	// Send the current set point based on the previous calculations
	this->sendSetpoint(m_fRoll, m_fPitch, m_fYaw, m_nThrust);
	m_dSetpointLastSent = dTimeNow;
      }
    } else {
      // Send a dummy packet for keepalive
      m_crRadio->sendDummyPacket();
    }
  } break;
    
  default: {
  } break;
  }
  
  if(m_crRadio->ackReceived()) {
    m_nAckMissCounter = 0;
  } else {
    m_nAckMissCounter++;
  }
  
  return m_crRadio->usbOK();
}

bool CCrazyflie::copterInRange() {
  return m_nAckMissCounter < m_nAckMissTolerance;
}

void CCrazyflie::setRoll(float fRoll) {
  m_fRoll = fRoll;
  
  if(fabs(m_fRoll) > m_fMaxAbsRoll) {
    m_fRoll = copysign(m_fMaxAbsRoll, m_fRoll);
  }
}

float CCrazyflie::roll() {
  return this->sensorDoubleValue("stabilizer.roll");
}

void CCrazyflie::setPitch(float fPitch) {
  m_fPitch = fPitch;
  
  if(fabs(m_fPitch) > m_fMaxAbsPitch) {
    m_fPitch = copysign(m_fMaxAbsPitch, m_fPitch);
  }
}

float CCrazyflie::pitch() {
  return this->sensorDoubleValue("stabilizer.pitch");
}

void CCrazyflie::setYaw(float fYaw) {
  m_fYaw = fYaw;
  
  if(m_fYaw < 0) {
    m_fYaw += 2 * M_PI - m_fYaw;
  }
  
  while(m_fYaw > m_fMaxYaw) {
    m_fYaw -= m_fMaxYaw;
  }
}

float CCrazyflie::yaw() {
  return this->sensorDoubleValue("stabilizer.yaw");
}

double CCrazyflie::currentTime() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  
  return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}

bool CCrazyflie::isInitialized() {
  return m_enumState == STATE_NORMAL_OPERATION;
}

bool CCrazyflie::startLogging() {
  // Register the desired sensor readings
  this->enableStabilizerLogging();
  this->enableGyroscopeLogging();
  this->enableAccelerometerLogging();
  this->enableBatteryLogging();
  this->enableMagnetometerLogging();
  this->enableAltimeterLogging();
  
  return true;
}

bool CCrazyflie::stopLogging() {
  this->disableStabilizerLogging();
  this->disableGyroscopeLogging();
  this->disableAccelerometerLogging();
  this->disableBatteryLogging();
  this->disableMagnetometerLogging();
  this->disableAltimeterLogging();
  
  return true;
}

void CCrazyflie::setSendSetpoints(bool bSendSetpoints) {
  m_bSendsSetpoints = bSendSetpoints;
}

bool CCrazyflie::sendsSetpoints() {
  return m_bSendsSetpoints;
}

double CCrazyflie::sensorDoubleValue(string strName) {
  return m_tocLogs->doubleValue(strName);
}

void CCrazyflie::disableLogging() {
  m_tocLogs->unregisterLoggingBlock("high-speed");
  m_tocLogs->unregisterLoggingBlock("low-speed");
}

void CCrazyflie::enableStabilizerLogging() {
  m_tocLogs->registerLoggingBlock("stabilizer", 1000);
  
  m_tocLogs->startLogging("stabilizer.roll", "stabilizer");
  m_tocLogs->startLogging("stabilizer.pitch", "stabilizer");
  m_tocLogs->startLogging("stabilizer.yaw", "stabilizer");
}

void CCrazyflie::enableGyroscopeLogging() {
  m_tocLogs->registerLoggingBlock("gyroscope", 1000);

  m_tocLogs->startLogging("gyro.x", "gyroscope");
  m_tocLogs->startLogging("gyro.y", "gyroscope");
  m_tocLogs->startLogging("gyro.z", "gyroscope");
}

float CCrazyflie::gyroX() {
  return this->sensorDoubleValue("gyro.x");
}

float CCrazyflie::gyroY() {
  return this->sensorDoubleValue("gyro.x");
}

float CCrazyflie::gyroZ() {
  return this->sensorDoubleValue("gyro.x");
}

void CCrazyflie::enableAccelerometerLogging() {
  m_tocLogs->registerLoggingBlock("accelerometer", 1000);

  m_tocLogs->startLogging("acc.x", "accelerometer");
  m_tocLogs->startLogging("acc.y", "accelerometer");
  m_tocLogs->startLogging("acc.z", "accelerometer");
  m_tocLogs->startLogging("acc.zw", "accelerometer");
}

float CCrazyflie::accX() {
  return this->sensorDoubleValue("acc.x");
}

float CCrazyflie::accY() {
  return this->sensorDoubleValue("acc.y");
}

float CCrazyflie::accZ() {
  return this->sensorDoubleValue("acc.z");
}

float CCrazyflie::accZW() {
  return this->sensorDoubleValue("acc.zw");
}

void CCrazyflie::disableStabilizerLogging() {
  m_tocLogs->unregisterLoggingBlock("stabilizer");
}

void CCrazyflie::disableGyroscopeLogging() {
  m_tocLogs->unregisterLoggingBlock("gyroscope");
}

void CCrazyflie::disableAccelerometerLogging() {
  m_tocLogs->unregisterLoggingBlock("accelerometer");
}

void CCrazyflie::enableBatteryLogging() {
  m_tocLogs->registerLoggingBlock("battery", 1000);

  m_tocLogs->startLogging("pm.vbat", "battery");
  m_tocLogs->startLogging("pm.state", "battery");
}

double CCrazyflie::batteryLevel() {
  return this->sensorDoubleValue("pm.vbat");
}

float CCrazyflie::batteryState() {
  return this->sensorDoubleValue("pm.state");
}

void CCrazyflie::disableBatteryLogging() {
  m_tocLogs->unregisterLoggingBlock("battery");
}

void CCrazyflie::enableMagnetometerLogging() {
  m_tocLogs->registerLoggingBlock("magnetometer", 1000);

  m_tocLogs->startLogging("mag.x", "magnetometer");
  m_tocLogs->startLogging("mag.y", "magnetometer");
  m_tocLogs->startLogging("mag.z", "magnetometer");
}
float CCrazyflie::magX() {
  return this->sensorDoubleValue("mag.x");
}
float CCrazyflie::magY() {
  return this->sensorDoubleValue("mag.y");
}
float CCrazyflie::magZ() {
  return this->sensorDoubleValue("mag.z");
}
void CCrazyflie::disableMagnetometerLogging() {
  m_tocLogs->unregisterLoggingBlock("magnetometer");
}



void CCrazyflie::enableAltimeterLogging() {
  m_tocLogs->registerLoggingBlock("altimeter", 1000);
  m_tocLogs->startLogging("alti.asl", "altimeter");
  m_tocLogs->startLogging("alti.aslLong", "altimeter");
  m_tocLogs->startLogging("alti.pressure", "altimeter");
  m_tocLogs->startLogging("alti.temperature", "altimeter");
}

float CCrazyflie::asl() {
  return this->sensorDoubleValue("alti.asl");
}
float CCrazyflie::aslLong() {
  return this->sensorDoubleValue("alti.aslLong");
}
float CCrazyflie::pressure() {
  return this->sensorDoubleValue("alti.pressure");
}
float CCrazyflie::temperature() {
  return this->sensorDoubleValue("alti.temperature");
}



void CCrazyflie::disableAltimeterLogging() {
  m_tocLogs->unregisterLoggingBlock("altimeter");
}
