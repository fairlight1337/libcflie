// Copyright (c) 2013, Jan Winkler <winkler@cs.uni-bremen.de>
// Copyright (c) 2014 Alexander Holler <holler@ahsoftware.de>
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

/// \author Jan Winkler
/// \author Alexander Holler

#include <algorithm>

#include "cflie/CTOC.h"

using namespace std;

bool CTOC::sendTOCPointerReset() {
  CCRTPPacket *crtpPacket = new CCRTPPacket(0x00, m_nPort);
  crtpPacket->setChannel(CCRTPPacket::ChannelTOC);
  CCRTPPacket *crtpReceived = m_crRadio->sendPacket(crtpPacket, true);
  
  if(crtpReceived) {
    delete crtpReceived;
    return true;
  }
  
  return false;
}

bool CTOC::requestMetaData() {
  bool bReturnvalue = false;
  
  CCRTPPacket *crtpPacket = new CCRTPPacket(0x01, m_nPort);
  crtpPacket->setChannel(CCRTPPacket::ChannelTOC);
  CCRTPPacket *crtpReceived = m_crRadio->sendAndReceive(crtpPacket);
  
  if(crtpReceived->payload()[1] == 0x01) {
    m_nItemCount = crtpReceived->payload()[2];
    bReturnvalue = true;
  }
  
  delete crtpReceived;
  return bReturnvalue;
}

bool CTOC::requestItem(int nID, bool bInitial) {
  bool bReturnvalue = false;
  
  char cRequest[2];
  cRequest[0] = 0x0;
  cRequest[1] = nID;
  
  CCRTPPacket *crtpPacket = new CCRTPPacket(cRequest,
					    (bInitial ? 1 : 2),
					    m_nPort);
  crtpPacket->setChannel(CCRTPPacket::ChannelTOC);
  CCRTPPacket *crtpReceived = m_crRadio->sendAndReceive(crtpPacket);
  
  bReturnvalue = this->processItem(crtpReceived);
  
  delete crtpReceived;
  return bReturnvalue;
}

bool CTOC::requestItems() {
  for(int nI = 0; nI < m_nItemCount; nI++) {
    this->requestItem(nI);
  }
  //this->requestInitialItem();
  
  return true;
}

bool CTOC::processItem(CCRTPPacket *crtpItem) {
  if(crtpItem->port() == m_nPort) {
    if(crtpItem->channel() == CCRTPPacket::ChannelTOC) {
      const char *cData = crtpItem->payload();

      if(cData[1] == 0x0) { // Command identification ok?
	uint8_t nID = cData[2];
	uint8_t nType = cData[3];
	
	string strGroup;
	int nI;
	for(nI = 4; cData[nI] != '\0'; nI++) {
	  strGroup += cData[nI];
	}
	
	nI++;
	string strIdentifier;
	for(; cData[nI] != '\0'; nI++) {
	  strIdentifier += cData[nI];
	}
	
	struct TOCElement teNew;
	teNew.strIdentifier = strIdentifier;
	teNew.strGroup = strGroup;
	teNew.nID = nID;
	teNew.nType = nType;
	teNew.bIsLogging = false;
	
	m_lstTOCElements.push_back(teNew);

	cout << (m_nPort == CCRTPPacket::PortParam ? "Parameter" : "Log" ) << " item ID " << int(nID) << ' ' << strGroup << "." << strIdentifier;
        if(m_nPort == CCRTPPacket::PortParam)
	  std::cout << " (" << getParameterTypeName(nType) << ", " <<
            (nType & 0xf0 ? "ro)" : "rw)");
        else if(m_nPort == CCRTPPacket::PortLogging)
	  std::cout << " (" << getLogTypeName(nType) << ')';
        std::cout << endl;

	if (m_nPort == CCRTPPacket::PortParam)
	  requestParameterValue(nID);

	return true;
      }
    }
  }
  
  return false;
}

struct TOCElement CTOC::elementForName(const std::string& strName, bool &bFound) const {
  for(list<struct TOCElement>::const_iterator itElement = m_lstTOCElements.begin();
      itElement != m_lstTOCElements.end();
      itElement++) {
    struct TOCElement teCurrent = *itElement;
    
    string strTempFullname = teCurrent.strGroup + "." + teCurrent.strIdentifier;
    if(strName == strTempFullname) {
      bFound = true;
      return teCurrent;
    }
  }
  
  bFound = false;
  struct TOCElement teEmpty;
  
  return teEmpty;
}

struct TOCElement CTOC::elementForID(uint8_t nID, bool &bFound) const {
  for(list<struct TOCElement>::const_iterator itElement = m_lstTOCElements.begin();
      itElement != m_lstTOCElements.end();
      itElement++) {
    struct TOCElement teCurrent = *itElement;
    
    if(nID == teCurrent.nID) {
      bFound = true;
      return teCurrent;
    }
  }
  
  bFound = false;
  struct TOCElement teEmpty;
  
  return teEmpty;
}

int CTOC::idForName(const std::string& strName) const {
  bool bFound;
  
  struct TOCElement teResult = elementForName(strName, bFound);
  
  if(bFound) {
    return teResult.nID;
  }
  
  return -1;
}

int CTOC::typeForName(const std::string& strName) const {
  bool bFound;
  
  struct TOCElement teResult = elementForName(strName, bFound);
  
  if(bFound) {
    return teResult.nType;
  }
  
  return -1;
}

bool CTOC::startLogging(const std::string& strName, const std::string& strBlockName) {
  bool bFound;
  struct LoggingBlock lbCurrent = this->loggingBlockForName(strBlockName, bFound);
  
  if(bFound) {
    struct TOCElement teCurrent = this->elementForName(strName, bFound);
    if(bFound) {
      char cPayload[5] = {0x01, (char)lbCurrent.nID, (char)teCurrent.nType, (char)teCurrent.nID};
      CCRTPPacket *crtpLogVariable = new CCRTPPacket(cPayload, 4, m_nPort);
      crtpLogVariable->setChannel(CCRTPPacket::ChannelRead);
      CCRTPPacket *crtpReceived = m_crRadio->sendAndReceive(crtpLogVariable, true);
      
      const char *cData = crtpReceived->payload();
      bool bCreateOK = false;
      if(cData[1] == 0x01 &&
	 cData[2] == lbCurrent.nID &&
	 cData[3] == 0x00) {
	bCreateOK = true;
      } else {
	cout << cData[3] << endl;
      }
      
      if(crtpReceived) {
	delete crtpReceived;
      }
      
      if(bCreateOK) {
	this->addElementToBlock(lbCurrent.nID, teCurrent.nID);
	
	return true;
      }
    }
  }
  
  return false;
}

bool CTOC::addElementToBlock(uint8_t nBlockID, uint8_t nElementID) {
  for(list<struct LoggingBlock>::iterator itBlock = m_lstLoggingBlocks.begin();
      itBlock != m_lstLoggingBlocks.end();
      itBlock++) {
    struct LoggingBlock lbCurrent = *itBlock;
    
    if(lbCurrent.nID == nBlockID) {
      (*itBlock).lstElementIDs.push_back(nElementID);
      
      return true;
    }
  }
  
  return false;
}

struct LoggingBlock CTOC::loggingBlockForName(const std::string& strName, bool &bFound) const {
  for(list<struct LoggingBlock>::const_iterator itBlock = m_lstLoggingBlocks.begin();
      itBlock != m_lstLoggingBlocks.end();
      itBlock++) {
    struct LoggingBlock lbCurrent = *itBlock;
    
    if(strName == lbCurrent.strName) {
      bFound = true;
      return lbCurrent;
    }
  }
  
  bFound = false;
  struct LoggingBlock lbEmpty;
  
  return lbEmpty;
}

struct LoggingBlock CTOC::loggingBlockForID(uint8_t nID, bool &bFound) const {
  for(list<struct LoggingBlock>::const_iterator itBlock = m_lstLoggingBlocks.begin();
      itBlock != m_lstLoggingBlocks.end();
      itBlock++) {
    struct LoggingBlock lbCurrent = *itBlock;
    
    if(nID == lbCurrent.nID) {
      bFound = true;
      return lbCurrent;
    }
  }
  
  bFound = false;
  struct LoggingBlock lbEmpty;
  
  return lbEmpty;
}

bool CTOC::registerLoggingBlock(const std::string& strName, double dFrequency) {
  uint8_t nID = 0;
  bool bFound;
  
  if(dFrequency > 0) { // Only do it if a valid frequency > 0 is given
    this->loggingBlockForName(strName, bFound);
    if(bFound) {
      this->unregisterLoggingBlock(strName);
    }
    
    do {
      this->loggingBlockForID(nID, bFound);
	
      if(bFound) {
	nID++;
      }
    } while(bFound);
    
    this->unregisterLoggingBlockID(nID);
    
    double d10thOfMS = (1 / dFrequency) * 1000 * 10;
    //    char cPayload[4] = {0x00, (nID >> 16) & 0x00ff, nID & 0x00ff, d10thOfMS};
    char cPayload[4] = {0x00, (char)nID, (char)d10thOfMS};
    CCRTPPacket *crtpRegisterBlock = new CCRTPPacket(cPayload, 3, m_nPort);
    crtpRegisterBlock->setChannel(CCRTPPacket::ChannelRead);
    
    CCRTPPacket *crtpReceived = m_crRadio->sendAndReceive(crtpRegisterBlock, true);
    
    const char *cData = crtpReceived->payload();
    bool bCreateOK = false;
    if(cData[1] == 0x00 &&
       cData[2] == nID &&
       cData[3] == 0x00) {
      bCreateOK = true;
      cout << "Registered logging block `" << strName << "'" << endl;
    }
    
    if(crtpReceived) {
      delete crtpReceived;
    }
      
    if(bCreateOK) {
      struct LoggingBlock lbNew;
      lbNew.strName = strName;
      lbNew.nID = nID;
      lbNew.dFrequency = dFrequency;
	
      m_lstLoggingBlocks.push_back(lbNew);
	
      return this->enableLogging(strName);
    }
  }
  
  return false;
}

bool CTOC::enableLogging(const std::string& strBlockName) {
  bool bFound;
  
  struct LoggingBlock lbCurrent = this->loggingBlockForName(strBlockName, bFound);
  if(bFound) {
    double d10thOfMS = (1 / lbCurrent.dFrequency) * 1000 * 10;
    char cPayload[3] = {0x03, (char)lbCurrent.nID, (char)d10thOfMS};
    
    CCRTPPacket *crtpEnable = new CCRTPPacket(cPayload, 3, m_nPort);
    crtpEnable->setChannel(CCRTPPacket::ChannelRead);
    
    CCRTPPacket *crtpReceived = m_crRadio->sendAndReceive(crtpEnable);
    delete crtpReceived;
    
    return true;
  }
  
  return false;
}

bool CTOC::unregisterLoggingBlock(const std::string& strName) {
  bool bFound;
  
  struct LoggingBlock lbCurrent = this->loggingBlockForName(strName, bFound);
  if(bFound) {
    return this->unregisterLoggingBlockID(lbCurrent.nID);
  }
  
  return false;
}

bool CTOC::unregisterLoggingBlockID(uint8_t nID) {
  char cPayload[2] = {0x02, (char)nID};
  CCRTPPacket *crtpUnregisterBlock = new CCRTPPacket(cPayload, 2, m_nPort);
  crtpUnregisterBlock->setChannel(CCRTPPacket::ChannelRead);
  CCRTPPacket *crtpReceived = m_crRadio->sendAndReceive(crtpUnregisterBlock, true);
  
  if(crtpReceived) {
    delete crtpReceived;
    return true;
  }
  
  return false;
}

void CTOC::processParameterPacket(CCRTPPacket& packet)
{
  if (packet.port() != CCRTPPacket::PortParam)
    return;
  if (packet.channel() == CCRTPPacket::ChannelTOC)
    return;
  const char *cData = packet.payload();
  uint8_t id = cData[1];
  auto i = std::find_if(m_lstTOCElements.begin(), m_lstTOCElements.end(), [id] (TOCElement e) { return e.nID == id; });
  if (i == m_lstTOCElements.end())
    throw std::runtime_error("Unknown parameter");
  if(packet.payloadLength() > 2 && packet.payloadLength()-2 <= sizeof(i->raw.raw))
    std::memcpy(i->raw.raw, cData+2, packet.payloadLength() - 2);
  else
    throw std::runtime_error("Broken parameter packet");
}

void CTOC::processLogPacket(CCRTPPacket& packet)
{
  if (packet.port() != CCRTPPacket::PortLogging)
    return;
  if (packet.channel() == CCRTPPacket::ChannelTOC) {
    std::cout << "foo" << std::endl;
    return;
  }
  const char *cData = packet.payload();
  bool bFound;
  int nBlockID = cData[1];
  struct LoggingBlock lbCurrent = loggingBlockForID(nBlockID, bFound);
  if(!bFound)
    throw std::runtime_error("Unknown logging block");
  unsigned nOffset = 0;
  unsigned nByteLength;
  for(unsigned nIndex = 0; nIndex < lbCurrent.lstElementIDs.size(); ++nIndex, nOffset += nByteLength) {
    int id = elementIDinBlock(nBlockID, nIndex);
    auto i = std::find_if(m_lstTOCElements.begin(), m_lstTOCElements.end(), [id] (TOCElement e) { return e.nID == id; });
    if (i == m_lstTOCElements.end())
      throw std::runtime_error("Unknown log element");
    nByteLength = sizeOfLogValue(i->nType);
    if(!nByteLength)
      throw std::runtime_error("Log value without size");
    if(packet.payloadLength() > 5 + nOffset && nByteLength <= sizeof(i->raw.raw))
      std::memcpy(i->raw.raw, cData+5+nOffset, nByteLength);
    else
      throw std::runtime_error("Broken log packet");
  }
}

void CTOC::processPackets(list<CCRTPPacket*> lstPackets) {
  if(lstPackets.empty())
    return;
  for(list<CCRTPPacket*>::iterator itPacket = lstPackets.begin();
	itPacket != lstPackets.end(); itPacket++) {
    CCRTPPacket *crtpPacket = *itPacket;

    if (crtpPacket->port() == CCRTPPacket::PortParam) {
      processParameterPacket(*crtpPacket);
      delete crtpPacket;
      continue;
    }

    if (crtpPacket->port() == CCRTPPacket::PortLogging) {
      processLogPacket(*crtpPacket);
      delete crtpPacket;
      continue;
    }
  }
}

int CTOC::elementIDinBlock(int nBlockID, unsigned nElementIndex) const {
  bool bFound;
  
  struct LoggingBlock lbCurrent = loggingBlockForID(nBlockID, bFound);
  if(bFound) {
    if(nElementIndex < lbCurrent.lstElementIDs.size()) {
      list<int>::const_iterator itID = lbCurrent.lstElementIDs.begin();
      advance(itID, nElementIndex);
      return *itID;
    }
  }
  
  return -1;
}

void CTOC::requestParameterValue(uint8_t id)
{
  CCRTPPacket *crtpPacket = new CCRTPPacket(id, CCRTPPacket::PortParam);
  crtpPacket->setChannel(CCRTPPacket::ChannelRead);
  CCRTPPacket *crtpReceived = m_crRadio->sendAndReceive(crtpPacket);
  if (!crtpReceived)
    throw std::runtime_error("No ack received");
  processParameterPacket(*crtpReceived);
  delete crtpReceived;
}
