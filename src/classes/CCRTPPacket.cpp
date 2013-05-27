#include "CCRTPPacket.h"


CCRTPPacket::CCRTPPacket(int nPort) {
  this->basicSetup();
  this->setPort(nPort);
}

CCRTPPacket::CCRTPPacket(char *cData, int nDataLength, int nPort) {
  this->basicSetup();
  this->setPort(nPort);
  
  this->setData(cData, nDataLength);
}

CCRTPPacket::~CCRTPPacket() {
}

void CCRTPPacket::basicSetup() {
  m_cData = NULL;
  m_nDataLength = 0;
  m_nPort = -1;
}

void CCRTPPacket::setData(char *cData, int nDataLength) {
  this->clearData();
  
  m_cData = new char[nDataLength]();
  memcpy(m_cData, cData, nDataLength);
  m_nDataLength = nDataLength;
}

char *CCRTPPacket::data() {
  return m_cData;
}

int CCRTPPacket::dataLength() {
  return m_nDataLength;
}

void CCRTPPacket::clearData() {
  if(m_cData != NULL) {
    delete[] m_cData;
    m_cData = NULL;
    m_nDataLength = 0;
  }
}

char *CCRTPPacket::sendableData() {
  char *cSendable = new char[m_nDataLength + 2]();
  
  cSendable[0] = (m_nPort << 4) | 0b00001100 | m_nChannel;
  memcpy(&cSendable[1], m_cData, m_nDataLength);
  cSendable[m_nDataLength + 1] = 0x27;
  
  return cSendable;
}

int CCRTPPacket::sendableDataLength() {
  return m_nDataLength + 2;
}

void CCRTPPacket::setPort(int nPort) {
  m_nPort = nPort;
}

int CCRTPPacket::port() {
  return m_nPort;
}

void CCRTPPacket::setChannel(int nChannel) {
  m_nChannel = nChannel;
}

int CCRTPPacket::channel() {
  return m_nChannel;
}
