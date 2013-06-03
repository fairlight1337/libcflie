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
  m_nLastRequestedVariableIndex = -1;
  m_nThrust = 0;
  m_ctrlController = NULL;
  m_dSecondsLast = this->currentTime();
  
  // Review these values
  m_fMaxAbsRoll = 45.0f;
  m_fMaxAbsPitch = m_fMaxAbsRoll;
  m_fMaxYaw = 2 * M_PI;
  m_nMaxThrust = 60000;
  m_nMinThrust = 0;//15000;

  m_dAccZZero = 0;
  
  m_fRoll = 0;
  m_fPitch = 0;
  m_fYaw = 0;
  m_nThrust = 0;
  
  m_bControllerIgnoresYaw = false;
  m_fArrivalThreshold = 0.05;
  m_bSendsSetpoints = false;
  
  this->disableController();
  
  //this->updateTOC();
  //this->updateLogTOC();
  m_tocParameters = new CTOC(m_crRadio, 2);
  m_tocLogs = new CTOC(m_crRadio, 5);
  
  m_enumState = STATE_ZERO;
  
  m_nAckMissCounter = 0;
  m_nAckMissTolerance = 10;
  
  m_dSendSetpointPeriod = 0.01; // Seconds
  m_dSetpointLastSent = 0;
  
  this->resetState();
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

void CCrazyflie::resetState() {
  m_dspCurrentPose.dsvPosition.fX = 0;
  m_dspCurrentPose.dsvPosition.fY = 0;
  m_dspCurrentPose.dsvPosition.fZ = 0;
  
  m_dspCurrentPose.dsoOrientation.fRoll = 0;
  m_dspCurrentPose.dsoOrientation.fPitch = 0;
  m_dspCurrentPose.dsoOrientation.fYaw = 0;
  
  m_dstCurrentTwist.dsvLinear.fX = 0;
  m_dstCurrentTwist.dsvLinear.fY = 0;
  m_dstCurrentTwist.dsvLinear.fZ = 0;
  
  m_dstCurrentTwist.dsoAngular.fRoll = 0;
  m_dstCurrentTwist.dsoAngular.fPitch = 0;
  m_dstCurrentTwist.dsoAngular.fYaw = 0;
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
  return m_nThrust;
}

bool CCrazyflie::cycle() {
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
    
    m_dAccZZero = this->sensorDoubleValue("acc.z");
    //cout << "!!!" << m_dAccZZero << endl;
    
    m_enumState = STATE_NORMAL_OPERATION;
  } break;
    
  case STATE_NORMAL_OPERATION: {
    // Handle time calculation
    double dTimeNow = this->currentTime();
    double dSecondsElapsed = m_dSecondsLast - dTimeNow;
    m_dSecondsLast = dTimeNow;
    
    // Shove over the sensor readings from the radio to the Logs TOC.
    m_tocLogs->processPackets(m_crRadio->popLoggingPackets());
    
    //cout << m_tocLogs->doubleValue("stabilizer.yaw") << endl;
    
    // Calculate the linear twist (i.e. cartesian velocity) from the
    // angular twist (RPY). This is mainly based on taking the current
    // angles of the device into account (from the current pose) and
    // then applying the values from the gyroscope.
    this->calculateCartesianVelocity(dSecondsElapsed);
    
    // Calculate pose integral and apply the calculated control signals
    this->calculatePoseIntegral(dSecondsElapsed);
    this->applyControllerResult(dSecondsElapsed);
    
    if(m_bSendsSetpoints) {
      // Check if it's time to send the setpoint
      if(dTimeNow - m_dSetpointLastSent > m_dSendSetpointPeriod) {
	// Send the current set point based on the previous calculations
	//cout << "Send " << m_nThrust << endl;
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
  return m_fRoll;
}

void CCrazyflie::setPitch(float fPitch) {
  m_fPitch = fPitch;
  
  if(fabs(m_fPitch) > m_fMaxAbsPitch) {
    m_fPitch = copysign(m_fMaxAbsPitch, m_fPitch);
  }
}

float CCrazyflie::pitch() {
  return m_fPitch;
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
  return m_fYaw;
}

void CCrazyflie::disableController() {
  m_enumCtrl = CTRL_NONE;
  
  if(m_ctrlController != NULL) {
    delete m_ctrlController;
    m_ctrlController = NULL;
  }
}

void CCrazyflie::setPController(float fPGain) {
  if(m_enumCtrl != CTRL_P) {
    this->disableController();
    
    m_enumCtrl = CTRL_P;
    m_ctrlController = new CPController();
    m_ctrlController->setIgnoresYaw(this->controllerIgnoresYaw());
    
    ((CPController*)m_ctrlController)->setPGain(fPGain);
  }
}

void CCrazyflie::setDesiredSetPoint(struct DSControlSetPoint cspDesired) {
  m_cspDesired = cspDesired;
}

double CCrazyflie::currentTime() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  
  return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}

void CCrazyflie::calculateCartesianVelocity(double dElapsedTime) {
  // TODO(winkler): Calculate the cartesian velocity from the angular
  // and gyro values here.
  double dRoll = this->sensorDoubleValue("stabilizer.roll");
  double dPitch = this->sensorDoubleValue("stabilizer.pitch");
  double dYaw = this->sensorDoubleValue("stabilizer.yaw");
  
  double dAccX = this->sensorDoubleValue("acc.x");
  double dAccY = this->sensorDoubleValue("acc.y");
  double dAccZ = this->sensorDoubleValue("acc.z");
  
  double dMagX = this->sensorDoubleValue("mag.x");
  double dMagY = this->sensorDoubleValue("mag.y");
  double dMagZ = this->sensorDoubleValue("mag.z");
  
  //cout << "Ang: " << dRoll << " " << dPitch << " " << dYaw << endl;
  //cout << "Acc: " << dAccX << " " << dAccY << " " << dAccZ << endl;
  //cout << "Mag: " << dMagX << " " << dMagY << " " << dMagZ << endl;
  
  //m_dstCurrentTwist.dsvLinear.fZ += (dAccZ - m_dAccZZero) * dElapsedTime;
}

void CCrazyflie::calculatePoseIntegral(double dElapsedTime) {
  m_dspCurrentPose.dsvPosition.fX += dElapsedTime * m_dstCurrentTwist.dsvLinear.fX;
  m_dspCurrentPose.dsvPosition.fY += dElapsedTime * m_dstCurrentTwist.dsvLinear.fY;
  m_dspCurrentPose.dsvPosition.fZ += dElapsedTime * m_dstCurrentTwist.dsvLinear.fZ;
  
  //cout << "Z: " << m_dspCurrentPose.dsvPosition.fZ << endl;
}

struct DSVelocityControlSignal CCrazyflie::identityControlSignal() {
  struct DSVelocityControlSignal dvcsIdentity;
  
  dvcsIdentity.nThrust = 0;
  dvcsIdentity.dsoAngular.fRoll = 0;
  dvcsIdentity.dsoAngular.fPitch = 0;
  dvcsIdentity.dsoAngular.fYaw = 0;
  
  return dvcsIdentity;
}

void CCrazyflie::applyControllerResult(double dElapsedTime) {
  struct DSVelocityControlSignal dvcsResult = this->identityControlSignal();
  
  switch(m_enumCtrl) {
  case CTRL_P: {
    dvcsResult = ((CPController*)m_ctrlController)->inputSignalForDesiredPosition(m_dspCurrentPose, m_cspDesired);
  } break;
      
  case CTRL_NONE:  
  default: {
    // Unknown or no controller, don't do anything.
  } break;
  }
    
  // Apply the relative velocity signal to the set point according
  // to the elapsed time
  this->setRoll(this->roll() +
		dElapsedTime * dvcsResult.dsoAngular.fRoll);
  this->setPitch(this->pitch() +
		 dElapsedTime * dvcsResult.dsoAngular.fPitch);
  this->setYaw(this->yaw() +
	       dElapsedTime * dvcsResult.dsoAngular.fYaw);
  this->setThrust(this->thrust() +
		  dElapsedTime * dvcsResult.nThrust);
}

double CCrazyflie::distanceBetweenPositions(struct DSVector dsvPosition1, struct DSVector dsvPosition2) {
  return sqrt(((dsvPosition1.fX - dsvPosition2.fX) *
	       (dsvPosition1.fX - dsvPosition2.fX)) +
	      ((dsvPosition1.fY - dsvPosition2.fY) *
	       (dsvPosition1.fY - dsvPosition2.fY)) +
	      ((dsvPosition1.fZ - dsvPosition2.fZ) *
	       (dsvPosition1.fZ - dsvPosition2.fZ)));
}

double CCrazyflie::distanceToPosition(struct DSVector dsvPosition) {
  return this->distanceBetweenPositions(dsvPosition,
					m_cspDesired.dsvPosition);
}

double CCrazyflie::distanceToDesiredPosition() {
  return this->distanceToPosition(m_dspCurrentPose.dsvPosition);
}

void CCrazyflie::goToRelativePosition(struct DSVector dsvRelative) {
  struct DSVector dsvAbsolute;
  
  dsvAbsolute = m_dspCurrentPose.dsvPosition;
  dsvAbsolute.fX += dsvRelative.fX;
  dsvAbsolute.fY += dsvRelative.fY;
  dsvAbsolute.fZ += dsvRelative.fZ;
  
  this->goToAbsolutePosition(dsvAbsolute);
}

void CCrazyflie::goToAbsolutePosition(struct DSVector dsvAbsolute) {
  struct DSControlSetPoint cspDesired;
  
  cspDesired.fYaw = m_dspCurrentPose.dsoOrientation.fYaw;
  cspDesired.dsvPosition = dsvAbsolute;
  
  this->setDesiredSetPoint(cspDesired);
}

void CCrazyflie::setControllerIgnoresYaw(bool bControllerIgnoresYaw) {
  if(m_ctrlController) {
    m_ctrlController->setIgnoresYaw(bControllerIgnoresYaw);
  }
  
  m_bControllerIgnoresYaw = bControllerIgnoresYaw;
}

bool CCrazyflie::controllerIgnoresYaw() {
  return m_bControllerIgnoresYaw;
}

bool CCrazyflie::isAtDesiredPosition() {
  return this->isAtDesiredPosition(m_fArrivalThreshold);
}

bool CCrazyflie::isAtDesiredPosition(float fThreshold) {
  return this->distanceToDesiredPosition() <= fThreshold;
}

void CCrazyflie::setArrivalThreshold(float fArrivalThreshold) {
  m_fArrivalThreshold = fArrivalThreshold;
}

float CCrazyflie::arrivalThreshold() {
  return m_fArrivalThreshold;
}

void CCrazyflie::relocalize() {
  this->resetState();
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

void CCrazyflie::enableAccelerometerLogging() {
  m_tocLogs->registerLoggingBlock("accelerometer", 1000);

  m_tocLogs->startLogging("acc.x", "accelerometer");
  m_tocLogs->startLogging("acc.y", "accelerometer");
  m_tocLogs->startLogging("acc.z", "accelerometer");
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

void CCrazyflie::disableMagnetometerLogging() {
  m_tocLogs->unregisterLoggingBlock("magnetometer");
}

double CCrazyflie::batteryLevel() {
  return this->sensorDoubleValue("pm.vbat");
}

void CCrazyflie::enableAltimeterLogging() {
  m_tocLogs->registerLoggingBlock("altimeter", 1000);

  m_tocLogs->startLogging("alti.pressure", "altimeter");
  m_tocLogs->startLogging("alti.temperature", "altimeter");
}

void CCrazyflie::disableAltimeterLogging() {
  m_tocLogs->unregisterLoggingBlock("altimeter");
}
