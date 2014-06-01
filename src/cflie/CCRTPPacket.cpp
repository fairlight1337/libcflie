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

#include <cstring> // memcpy

#include "cflie/CCRTPPacket.h"


CCRTPPacket::CCRTPPacket(int nPort) {
  this->basicSetup();
  this->setPort(nPort);
}

CCRTPPacket::CCRTPPacket(const char *cData, int nDataLength, int nPort)
  : m_cData(cData, nDataLength)
{
  this->basicSetup();
  this->setPort(nPort);
}

CCRTPPacket::CCRTPPacket(char cData, int nPort)
  : m_cData(&cData, 1)
{
  this->basicSetup();
  this->setPort(nPort);
}

void CCRTPPacket::basicSetup() {
  m_nPort = 0;
  m_nChannel = 0;
  m_bIsPingPacket = false;
}

char *CCRTPPacket::sendableData() {
  char *cSendable = new char[this->sendableDataLength()]();
  
  if(m_bIsPingPacket) {
    cSendable[0] = 0xff;
  } else {
    // Header byte
    cSendable[0] = (m_nPort << 4) | 0b00001100 | (m_nChannel & 0x03);
    
    // Payload
    std::memcpy(&cSendable[1], m_cData.data(), m_cData.size());
    
    // Finishing byte
    //cSendable[m_nDataLength + 1] = 0x27;
  }
  
  return cSendable;
}

int CCRTPPacket::sendableDataLength() {
  if(m_bIsPingPacket) {
    return 1;
  } else {
    return m_cData.size() + 1;//2;
  }
}
