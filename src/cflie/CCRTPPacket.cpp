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

#include "cflie/CCRTPPacket.h"


CCRTPPacket::CCRTPPacket(int nPort) {
  this->basicSetup();
  this->setPort(nPort);
}

CCRTPPacket::CCRTPPacket(char *cData, int nDataLength, int nPort) {
  this->basicSetup();
  this->setPort(nPort);
  
  this->setData(cData, nDataLength);
}

CCRTPPacket::CCRTPPacket(char cData, int nPort) {
  this->basicSetup();
  this->setPort(nPort);
  
  this->setData(&cData, 1);
}

CCRTPPacket::~CCRTPPacket() {
  this->clearData();
}

void CCRTPPacket::basicSetup() {
  m_cData = NULL;
  m_nDataLength = 0;
  m_nPort = 0;
  m_nChannel = 0;
  m_bIsPingPacket = false;
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
  char *cSendable = new char[this->sendableDataLength()]();
  
  if(m_bIsPingPacket) {
    cSendable[0] = 0xff;
  } else {
    // Header byte
    cSendable[0] = (m_nPort << 4) | 0b00001100 | (m_nChannel & 0x03);
    
    // Payload
    memcpy(&cSendable[1], m_cData, m_nDataLength);
    
    // Finishing byte
    //cSendable[m_nDataLength + 1] = 0x27;
  }
  
  return cSendable;
}

int CCRTPPacket::sendableDataLength() {
  if(m_bIsPingPacket) {
    return 1;
  } else {
    return m_nDataLength + 1;//2;
  }
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

void CCRTPPacket::setIsPingPacket(bool bIsPingPacket) {
  m_bIsPingPacket = bIsPingPacket;
}

bool CCRTPPacket::isPingPacket() {
  return m_bIsPingPacket;
}
