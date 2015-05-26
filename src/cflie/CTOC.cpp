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


#include <cflie/CTOC.h>


CTOC::CTOC(CCrazyRadio *crRadio, int nPort) {
  m_crRadio = crRadio;
  m_nPort = nPort;
  m_nItemCount = 0;
}

CTOC::~CTOC() {
}

bool CTOC::sendTOCPointerReset() {
  CCRTPPacket* crtpPacket = new CCRTPPacket(0x00, 0);
  crtpPacket->setPort(m_nPort);
  CCRTPPacket* crtpReceived = m_crRadio->sendPacket(crtpPacket, true);

  if(crtpReceived) {
    delete crtpReceived;
    return true;
  }

  return false;
}

bool CTOC::requestMetaData() {
  bool bReturnvalue = false;

  CCRTPPacket* crtpPacket = new CCRTPPacket(0x01, 0);
  crtpPacket->setPort(m_nPort);
  CCRTPPacket* crtpReceived = m_crRadio->sendAndReceive(crtpPacket);

  if(crtpReceived->data()[1] == 0x01) {
    m_nItemCount = crtpReceived->data()[2];
    bReturnvalue = true;
  }

  delete crtpReceived;
  return bReturnvalue;
}

bool CTOC::requestInitialItem() {
  return this->requestItem(0, true);
}

bool CTOC::requestItem(int nID) {
  return this->requestItem(nID, false);
}

bool CTOC::requestItem(int nID, bool bInitial) {
  bool bReturnvalue = false;

  char cRequest[2];
  cRequest[0] = 0x0;
  cRequest[1] = nID;

  CCRTPPacket* crtpPacket = new CCRTPPacket(cRequest,
					    (bInitial ? 1 : 2),
					    0);
  crtpPacket->setPort(m_nPort);
  CCRTPPacket* crtpReceived = m_crRadio->sendAndReceive(crtpPacket);

  bReturnvalue = this->processItem(crtpReceived);

  delete crtpReceived;
  return bReturnvalue;
}

bool CTOC::requestItems() {
  for(int nI = 0; nI < m_nItemCount; nI++) {
    this->requestItem(nI);
  }

  return true;
}

bool CTOC::processItem(CCRTPPacket* crtpItem) {
  if(crtpItem->port() == m_nPort) {
    if(crtpItem->channel() == 0) {
      char* cData = crtpItem->data();
      int nLength = crtpItem->dataLength();

      if(cData[1] == 0x0) { // Command identification ok?
	int nID = cData[2];
	int nType = cData[3];

	std::string strGroup;
	int nI;
	for(nI = 4; cData[nI] != '\0'; nI++) {
	  strGroup += cData[nI];
	}

	nI++;
	std::string strIdentifier;
	for(; cData[nI] != '\0'; nI++) {
	  strIdentifier += cData[nI];
	}

	struct TOCElement teNew;
	teNew.strIdentifier = strIdentifier;
	teNew.strGroup = strGroup;
	teNew.nID = nID;
	teNew.nType = nType;
	teNew.bIsLogging = false;
	teNew.dValue = 0;

	m_lstTOCElements.push_back(teNew);

	// NOTE(winkler): For debug purposes only.
	//std::cout << strGroup << "." << strIdentifier << std::endl;

	return true;
      }
    }
  }

  return false;
}

struct TOCElement CTOC::elementForName(std::string strName, bool& bFound) {
  for(std::list<struct TOCElement>::iterator itElement = m_lstTOCElements.begin();
      itElement != m_lstTOCElements.end();
      itElement++) {
    struct TOCElement teCurrent = *itElement;

    std::string strTempFullname = teCurrent.strGroup + "." + teCurrent.strIdentifier;
    if(strName == strTempFullname) {
      bFound = true;
      return teCurrent;
    }
  }

  bFound = false;
  struct TOCElement teEmpty;

  return teEmpty;
}

struct TOCElement CTOC::elementForID(int nID, bool& bFound) {
  for(std::list<struct TOCElement>::iterator itElement = m_lstTOCElements.begin();
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

int CTOC::idForName(std::string strName) {
  bool bFound;

  struct TOCElement teResult = this->elementForName(strName, bFound);

  if(bFound) {
    return teResult.nID;
  }

  return -1;
}

int CTOC::typeForName(std::string strName) {
  bool bFound;

  struct TOCElement teResult = this->elementForName(strName, bFound);

  if(bFound) {
    return teResult.nType;
  }

  return -1;
}

bool CTOC::startLogging(std::string strName, std::string strBlockName) {
  bool bFound;
  struct LoggingBlock lbCurrent = this->loggingBlockForName(strBlockName, bFound);

  if(bFound) {
    struct TOCElement teCurrent = this->elementForName(strName, bFound);
    if(bFound) {
      char cPayload[5] = {0x01, lbCurrent.nID, teCurrent.nType, teCurrent.nID};
      CCRTPPacket* crtpLogVariable = new CCRTPPacket(cPayload, 4, 1);
      crtpLogVariable->setPort(m_nPort);
      crtpLogVariable->setChannel(1);
      CCRTPPacket* crtpReceived = m_crRadio->sendAndReceive(crtpLogVariable, true);

      char* cData = crtpReceived->data();
      bool bCreateOK = false;
      if(cData[1] == 0x01 &&
	 cData[2] == lbCurrent.nID &&
	 cData[3] == 0x00) {
	bCreateOK = true;
      } else {
	std::cout << cData[3] << std::endl;
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

bool CTOC::addElementToBlock(int nBlockID, int nElementID) {
  for(std::list<struct LoggingBlock>::iterator itBlock = m_lstLoggingBlocks.begin();
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

bool CTOC::stopLogging(std::string strName) {
  // TODO: Implement me.
}

bool CTOC::isLogging(std::string strName) {
  // TODO: Implement me.
}

double CTOC::doubleValue(std::string strName) {
  bool bFound;

  struct TOCElement teResult = this->elementForName(strName, bFound);

  if(bFound) {
    return teResult.dValue;
  }

  return 0;
}

struct LoggingBlock CTOC::loggingBlockForName(std::string strName, bool& bFound) {
  for(std::list<struct LoggingBlock>::iterator itBlock = m_lstLoggingBlocks.begin();
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

struct LoggingBlock CTOC::loggingBlockForID(int nID, bool& bFound) {
  for(std::list<struct LoggingBlock>::iterator itBlock = m_lstLoggingBlocks.begin();
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

bool CTOC::registerLoggingBlock(std::string strName, double dFrequency) {
  int nID = 0;
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
    char cPayload[4] = {0x00, nID, d10thOfMS};

    CCRTPPacket* crtpRegisterBlock = new CCRTPPacket(cPayload, 3, 1);
    crtpRegisterBlock->setPort(m_nPort);
    crtpRegisterBlock->setChannel(1);

    CCRTPPacket* crtpReceived = m_crRadio->sendAndReceive(crtpRegisterBlock, true);

    char* cData = crtpReceived->data();
    bool bCreateOK = false;
    if(cData[1] == 0x00 &&
       cData[2] == nID &&
       cData[3] == 0x00) {
      bCreateOK = true;
      std::cout << "Registered logging block `" << strName << "'" << std::endl;
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

bool CTOC::enableLogging(std::string strBlockName) {
  bool bFound;

  struct LoggingBlock lbCurrent = this->loggingBlockForName(strBlockName, bFound);
  if(bFound) {
    double d10thOfMS = (1 / lbCurrent.dFrequency) * 1000 * 10;
    char cPayload[3] = {0x03, lbCurrent.nID, d10thOfMS};

    CCRTPPacket* crtpEnable = new CCRTPPacket(cPayload, 3, 1);
    crtpEnable->setPort(m_nPort);
    crtpEnable->setChannel(1);

    CCRTPPacket* crtpReceived = m_crRadio->sendAndReceive(crtpEnable);
    delete crtpReceived;

    return true;
  }

  return false;
}

bool CTOC::unregisterLoggingBlock(std::string strName) {
  bool bFound;

  struct LoggingBlock lbCurrent = this->loggingBlockForName(strName, bFound);
  if(bFound) {
    return this->unregisterLoggingBlockID(lbCurrent.nID);
  }

  return false;
}

bool CTOC::unregisterLoggingBlockID(int nID) {
  char cPayload[2] = {0x02, nID};

  CCRTPPacket* crtpUnregisterBlock = new CCRTPPacket(cPayload, 2, 1);
  crtpUnregisterBlock->setPort(m_nPort);
  crtpUnregisterBlock->setChannel(1);

  CCRTPPacket* crtpReceived = m_crRadio->sendAndReceive(crtpUnregisterBlock, true);

  if(crtpReceived) {
    delete crtpReceived;
    return true;
  }

  return false;
}

void CTOC::processPackets(std::list<CCRTPPacket*> lstPackets) {
  if(lstPackets.size() > 0) {
    for(std::list<CCRTPPacket*>::iterator itPacket = lstPackets.begin();
	itPacket != lstPackets.end();
	itPacket++) {
      CCRTPPacket* crtpPacket = *itPacket;

      char* cData = crtpPacket->data();
      float fValue;
      memcpy(&fValue, &cData[5], 4);

      char* cLogdata = &cData[5];
      int nOffset = 0;
      int nIndex = 0;
      int nAvailableLogBytes = crtpPacket->dataLength() - 5;

      int nBlockID = cData[1];
      bool bFound;
      struct LoggingBlock lbCurrent = this->loggingBlockForID(nBlockID, bFound);

      if(bFound) {
	while(nIndex < lbCurrent.lstElementIDs.size()) {
	  int nElementID = this->elementIDinBlock(nBlockID, nIndex);
	  bool bFound;
	  struct TOCElement teCurrent = this->elementForID(nElementID, bFound);

	  if(bFound) {
	    int nByteLength = 0;

	    // NOTE(winkler): We just copy over the incoming bytes in
	    // their according data structures and afterwards assign
	    // the value to fValue. This way, we let the compiler to
	    // the magic of conversion.
	    float fValue = 0;

	    switch(teCurrent.nType) {
	    case 1: { // UINT8
	      nByteLength = 1;
	      uint8_t uint8Value;
	      memcpy(&uint8Value, &cLogdata[nOffset], nByteLength);
	      fValue = uint8Value;
	    } break;

	    case 2: { // UINT16
	      nByteLength = 2;
	      uint16_t uint16Value;
	      memcpy(&uint16Value, &cLogdata[nOffset], nByteLength);
	      fValue = uint16Value;
	    } break;

	    case 3: { // UINT32
	      nByteLength = 4;
	      uint32_t uint32Value;
	      memcpy(&uint32Value, &cLogdata[nOffset], nByteLength);
	      fValue = uint32Value;
	    } break;

	    case 4: { // INT8
	      nByteLength = 1;
	      int8_t int8Value;
	      memcpy(&int8Value, &cLogdata[nOffset], nByteLength);
	      fValue = int8Value;
	    } break;

	    case 5: { // INT16
	      nByteLength = 2;
	      int16_t int16Value;
	      memcpy(&int16Value, &cLogdata[nOffset], nByteLength);
	      fValue = int16Value;
	    } break;

	    case 6: { // INT32
	      nByteLength = 4;
	      int32_t int32Value;
	      memcpy(&int32Value, &cLogdata[nOffset], nByteLength);
	      fValue = int32Value;
	    } break;

	    case 7: { // FLOAT
	      nByteLength = 4;
	      memcpy(&fValue, &cLogdata[nOffset], nByteLength);
	    } break;

	    case 8: { // FP16
	      // NOTE(winkler): This is untested code (as no FP16
	      // variable gets advertised yet). This has to be tested
	      // and is to be used carefully. I will do that as soon
	      // as I find time for it.
	      nByteLength = 2;
	      char cBuffer1[nByteLength];
	      char cBuffer2[4];
	      memcpy(cBuffer1, &cLogdata[nOffset], nByteLength);
	      cBuffer2[0] = cBuffer1[0] & 0b10000000; // Get the sign bit
	      cBuffer2[1] = 0;
	      cBuffer2[2] = cBuffer1[0] & 0b01111111; // Get the magnitude
	      cBuffer2[3] = cBuffer1[1];
	      memcpy(&fValue, cBuffer2, 4); // Put it into the float variable
	    } break;

	    default: { // Unknown. This hopefully never happens.
	    } break;
	    }

	    this->setFloatValueForElementID(nElementID, fValue);
	    nOffset += nByteLength;
	    nIndex++;
	  } else {
	    std::cerr << "Didn't find element ID " << nElementID
		 << " in block ID " << nBlockID
		 << " while parsing incoming logging data." << std::endl;
	    std::cerr << "This REALLY shouldn't be happening!" << std::endl;
	    std::exit(-1);
	  }
	}
      }

      delete crtpPacket;
    }
  }
}

int CTOC::elementIDinBlock(int nBlockID, int nElementIndex) {
  bool bFound;

  struct LoggingBlock lbCurrent = this->loggingBlockForID(nBlockID, bFound);
  if(bFound) {
    if(nElementIndex < lbCurrent.lstElementIDs.size()) {
      std::list<int>::iterator itID = lbCurrent.lstElementIDs.begin();
      advance(itID, nElementIndex);
      return *itID;
    }
  }

  return -1;
}

bool CTOC::setFloatValueForElementID(int nElementID, float fValue) {
  int nIndex = 0;
  for(std::list<struct TOCElement>::iterator itElement = m_lstTOCElements.begin();
      itElement != m_lstTOCElements.end();
      itElement++, nIndex++) {
    struct TOCElement teCurrent = *itElement;

    if(teCurrent.nID == nElementID) {
      teCurrent.dValue = fValue; // We store floats as doubles
      (*itElement) = teCurrent;
      // std::cout << fValue << std::endl;
      return true;
    }
  }

  return false;
}
