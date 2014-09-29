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


#ifndef __C_TOC_H__
#define __C_TOC_H__


#include <list>
#include <string>
#include <stdlib.h>
#include <cstring> // std::memcpy

#include "CCrazyRadio.h"
#include "CCRTPPacket.h"

union RawValue {
    char raw[8];
    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
    float f;
    double d;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
};

/*! \brief Storage element for logged variable identities */
struct TOCElement {
  /*! \brief The numerical ID of the log element on the copter's
      internal table */
  uint8_t nID;
  /*! \brief The (ref) type of the log element */
  uint8_t nType;
  /*! \brief The string group name of the log element */
  std::string strGroup;
  /*! \brief The string identifier of the log element */
  std::string strIdentifier;
  bool bIsLogging;
  double dValue;
  RawValue raw;
};


struct LoggingBlock {
  std::string strName;
  uint8_t nID;
  double dFrequency;
  std::list<int> lstElementIDs;
};


class CTOC {
 private:
  CCRTPPacket::Port m_nPort;
  CCrazyRadio *m_crRadio;
  int m_nItemCount;
  std::list<struct TOCElement> m_lstTOCElements;
  std::list<struct LoggingBlock> m_lstLoggingBlocks;
  
  bool requestInitialItem() {
    return this->requestItem(0, true);
  }
  bool requestItem(int nID, bool bInitial);
  bool requestItem(int nID) {
    return this->requestItem(nID, false);
  }
  bool processItem(CCRTPPacket *crtpItem);
  
  CCRTPPacket *sendAndReceive(CCRTPPacket *crtpSend, CCRTPPacket::Channel nChannel);

  bool checkParameter(uint8_t type, const int8_t&) const {
    return (type & 0x0f) == 0x00;
  }
  bool checkParameter(uint8_t type, const int16_t&) const {
    return (type & 0x0f) == 0x01;
  }
  bool checkParameter(uint8_t type, const int32_t&) const {
    return (type & 0x0f) == 0x02;
  }
  bool checkParameter(uint8_t type, const int64_t&) const {
    return (type & 0x0f) == 0x03;
  }
  // TODO: FP16 (type & 0x0f == 0x05)
  bool checkParameter(uint8_t type, const float&) const {
    return (type & 0x0f) == 0x06;
  }
  bool checkParameter(uint8_t type, const double&) const {
    return (type & 0x0f) == 0x07;
  }
  bool checkParameter(uint8_t type, const uint8_t&) const {
    return (type & 0x0f) == 0x08;
  }
  bool checkParameter(uint8_t type, const uint16_t&) const {
    return (type & 0x0f) == 0x09;
  }
  bool checkParameter(uint8_t type, const uint32_t&) const {
    return (type & 0x0f) == 0x0a;
  }
  bool checkParameter(uint8_t type, const uint64_t&) const {
    return (type & 0x0f) == 0x0b;
  }

  bool getAndCheckParameter(uint8_t type, int8_t& value, const RawValue& r) const {
    value = r.i8;
    return checkParameter(type, value);
  }
  bool getAndCheckParameter(uint8_t type, int16_t& value, const RawValue& r) const {
    value = r.i16;
    return checkParameter(type, value);
  }
  bool getAndCheckParameter(uint8_t type, int32_t& value, const RawValue& r) const {
    value = r.i32;
    return checkParameter(type, value);
  }
  bool getAndCheckParameter(uint8_t type, int64_t& value, const RawValue& r) const {
    value = r.i64;
    return checkParameter(type, value);
  }
  bool getAndCheckParameter(uint8_t type, float& value, const RawValue& r) const {
    value = r.f;
    return checkParameter(type, value);
  }
  bool getAndCheckParameter(uint8_t type, double& value, const RawValue& r) const {
    value = r.d;
    return checkParameter(type, value);
  }
  bool getAndCheckParameter(uint8_t type, uint8_t& value, const RawValue& r) const {
    value = r.u8;
    return checkParameter(type, value);
  }
  bool getAndCheckParameter(uint8_t type, uint16_t& value, const RawValue& r) const {
    value = r.u16;
    return checkParameter(type, value);
  }
  bool getAndCheckParameter(uint8_t type, uint32_t& value, const RawValue& r) const {
    value = r.u32;
    return checkParameter(type, value);
  }
  bool getAndCheckParameter(uint8_t type, uint64_t& value, const RawValue& r) const {
    value = r.u64;
    return checkParameter(type, value);
  }

  int requestParameterValue(uint8_t id);

 public:
  CTOC(CCrazyRadio *crRadio, CCRTPPacket::Port nPort)
    : m_nPort(nPort)
    , m_crRadio(crRadio)
    , m_nItemCount(0)
  {}

  bool sendTOCPointerReset();
  bool requestMetaData();
  bool requestItems();
  
  struct TOCElement elementForName(const std::string& strName, bool &bFound) const;
  struct TOCElement elementForID(uint8_t nID, bool &bFound) const;
  int idForName(const std::string& strName) const;
  int typeForName(const std::string& strName) const;

  // Set parameter values
  template <typename t>
  int setParameterValue(const std::string& strName, t value) {
    int8_t id(idForName(strName));
    if (id == -1)
      return 1;
    int type(typeForName(strName));
    if (type & 0xf0)
      return 2; // read only or group
    if (!checkParameter(type, value))
      return 3;
    char cData[1 + sizeof(value)];
    cData[0] = id;
    std::memcpy(cData+1, &value, sizeof(value));
    CCRTPPacket *crtpPacket = new CCRTPPacket(cData, sizeof(cData), CCRTPPacket::PortParam);
    crtpPacket->setChannel(CCRTPPacket::ChannelWrite);
    CCRTPPacket *crtpReceived = m_crRadio->sendPacket(crtpPacket, true);
    if(crtpReceived) {
      delete crtpReceived;
      return 0;
    }
    return 4;
  }
  // Get parameter values
  template <typename t>
  int getParameterValue(const std::string& strName, t& value) const {
    bool bFound;
    struct TOCElement teResult = elementForName(strName, bFound);
    if(!bFound)
      return 1;
    uint8_t type(typeForName(strName));
    if (!getAndCheckParameter(type, value, teResult.raw))
      return 2;
    return 0;
  }
  int requestParameterValue(const std::string& strName) {
    int8_t id(idForName(strName));
    if (id == -1)
      return 2;
    return requestParameterValue(id);
  }

  // For loggable variables only
  bool registerLoggingBlock(const std::string& strName, double dFrequency);
  bool unregisterLoggingBlock(const std::string& strName);
  struct LoggingBlock loggingBlockForName(const std::string& strName, bool &bFound) const;
  struct LoggingBlock loggingBlockForID(uint8_t nID, bool &bFound) const;
  
  bool startLogging(const std::string& strName, const std::string& strBlockName);
  bool stopLogging(const std::string& /*strName*/) {
    return true; // TODO
  }
  bool isLogging(const std::string& /*strName*/) {
    return true; // TODO
  }

  double doubleValue(const std::string& strName) const;
  
  bool enableLogging(const std::string& strBlockName);
  
  void processParameterPacket(CCRTPPacket& packet);
  void processPackets(std::list<CCRTPPacket*> lstPackets);
  
  int elementIDinBlock(int nBlockID, unsigned nElementIndex) const;
  bool setFloatValueForElementID(int nElementID, float fValue);
  bool addElementToBlock(uint8_t nBlockID, uint8_t nElementID);
  bool unregisterLoggingBlockID(uint8_t nID);
};


#endif /* __C_TOC_H__ */
