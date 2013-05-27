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
  m_dSecondsLast = 0;
  
  // Review these values
  m_fMaxAbsRoll = 0.5;
  m_fMaxAbsPitch = m_fMaxAbsRoll;
  m_fMaxYaw = 2 * M_PI;
  m_nMaxThrust = 60000;
  m_nMinThrust = 15000;
  
  this->disableController();
  
  //this->updateTOC();
  this->updateLogTOC();
}

CCrazyflie::~CCrazyflie() {
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
  
  if(crtpPacket != NULL) {
    delete crtpPacket;
    return true;
  } else {
    return false;
  }
}

void CCrazyflie::updateTOC() {
  char cOne = 1;
  m_crRadio->setUpdatesParameterCount(true);
  
  CCRTPPacket *crtpPacket = new CCRTPPacket(&cOne, 1, 2);
  CCRTPPacket *crtpReceived = m_crRadio->sendPacket(crtpPacket);
  
  delete crtpPacket;
  if(crtpReceived) {
    delete crtpReceived;
  }
}

void CCrazyflie::populateTOCElement(int nIndex) {
  char cRequest[2];
  cRequest[0] = 0;
  cRequest[1] = nIndex;
  cout << (int)cRequest[1] << endl;
  CCRTPPacket *crtpPacket = new CCRTPPacket(cRequest, 2, 2);
  CCRTPPacket *crtpReceived = m_crRadio->sendPacket(crtpPacket);
  
  delete crtpPacket;
  if(crtpReceived) {
    delete crtpReceived;
  }
}

void CCrazyflie::populateNextTOCElement() {
  if(m_crRadio->parameterCount() > 0) {
    m_nLastRequestedVariableIndex++;
    
    if(m_nLastRequestedVariableIndex < m_crRadio->parameterCount()) {
      this->populateTOCElement(m_nLastRequestedVariableIndex);
    }
  }
}

void CCrazyflie::updateLogTOC() {
  char cOne = 1;
  m_crRadio->setUpdatesLogParameterCount(true);
  m_crRadio->setLogElementPopulated(true);
  
  CCRTPPacket *crtpPacket = new CCRTPPacket(&cOne, 1, 5);
  crtpPacket->setChannel(0);
  
  CCRTPPacket *crtpReceived = m_crRadio->sendPacket(crtpPacket);
  
  delete crtpPacket;
  if(crtpReceived) {
    delete crtpReceived;
  }
}

void CCrazyflie::populateLOGElement(int nIndex) {
  char cRequest[2];
  cRequest[0] = 0;
  cRequest[1] = nIndex;
  cout << nIndex << ", " << (m_crRadio->countLOGElements() == 0 ? 1 : 2) << endl;
  CCRTPPacket *crtpPacket = new CCRTPPacket(cRequest, (m_crRadio->countLOGElements() == 0 ? 1 : 2), 2);
  crtpPacket->setChannel(2);
  CCRTPPacket *crtpReceived = m_crRadio->sendPacket(crtpPacket);
  
  delete crtpPacket;
  if(crtpReceived) {
    delete crtpReceived;
  }
}

void CCrazyflie::populateNextLOGElement() {
  if(m_crRadio->populatesLOGCache() && m_crRadio->logElementPopulated()) {
    int nNextLogVarID = m_crRadio->nextLogVarID();
    cout << nNextLogVarID << endl;
    if(nNextLogVarID != 0xff) {
      m_crRadio->setLogElementPopulated(false);
      this->populateLOGElement(nNextLogVarID);
    } else {
      m_crRadio->setPopulatesLOGCache(false);
    }
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

void CCrazyflie::cycle() {
  if(m_crRadio->populatesTOCCache()) {
    //this->populateNextTOCElement();
  }
  
  if(m_crRadio->populatesLOGCache()) {
    this->populateNextLOGElement();
  }
  
  this->applyControllerResult();
  this->sendSetpoint(m_fRoll, m_fPitch, m_fYaw, m_nThrust);
}

void CCrazyflie::setRoll(float fRoll) {
  m_fRoll = fRoll;
  
  if(fabs(m_fRoll) > m_fMaxAbsRoll) {
    m_fRoll = copysign(m_fRoll, m_fMaxAbsRoll);
  }
}

float CCrazyflie::roll() {
  return m_fRoll;
}

void CCrazyflie::setPitch(float fPitch) {
  m_fPitch = fPitch;
  
  if(fabs(m_fPitch) > m_fMaxAbsPitch) {
    m_fPitch = copysign(m_fPitch, m_fMaxAbsPitch);
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
    ((CPController*)m_ctrlController)->setPGain(fPGain);
  }
}

void CCrazyflie::setDesiredSetPoint(struct DSControlSetPoint cspDesired) {
  m_cspDesired = cspDesired;
}

void CCrazyflie::applyControllerResult() {
  struct DSVelocityControlSignal dvcsResult;
  
  struct timeval  tv;
  gettimeofday(&tv, NULL);
  double dTimeNow = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
  
  if(m_dSecondsLast > 0) {
    double dSecondsElapsed = m_dSecondsLast - dTimeNow;
    
    if(m_enumCtrl != CTRL_NONE) {
      switch(m_enumCtrl) {
      case CTRL_P: {
	dvcsResult = ((CPController*)m_ctrlController)->inputSignalForDesiredPose(m_dspCurrentPose, m_cspDesired);
      } break;
	
      case CTRL_NONE:  
      default: {
	// Unknown controller, don't do anything.
      } break;
      }
      
      // Apply the relative velocity signal to the set point according to the elapsed time
      this->setRoll(this->roll() + dSecondsElapsed * dvcsResult.dsoAngular.fRoll);
      this->setPitch(this->pitch() + dSecondsElapsed * dvcsResult.dsoAngular.fPitch);
      this->setYaw(this->yaw() + dSecondsElapsed * dvcsResult.dsoAngular.fYaw);
      this->setThrust(this->thrust() + dSecondsElapsed * dvcsResult.nThrust);
    }
  }
  
  m_dSecondsLast = dTimeNow;
}
