#include "cflie/CCrazyflie.h"


CCrazyflie::CCrazyflie(CCrazyRadio *crRadio) {
  m_crRadio = crRadio;
  m_nLastRequestedVariableIndex = -1;
  m_nThrust = 0;
  
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
}

int CCrazyflie::thrust() {
  return m_nThrust;
}

void CCrazyflie::cycle() {
  this->sendSetpoint(0, 0, 0, m_nThrust);
  
  if(m_crRadio->populatesTOCCache()) {
    //this->populateNextTOCElement();
  }
  
  if(m_crRadio->populatesLOGCache()) {
    this->populateNextLOGElement();
  }
}
