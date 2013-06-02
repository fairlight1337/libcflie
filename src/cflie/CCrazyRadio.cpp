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

#include "cflie/CCrazyRadio.h"


CCrazyRadio::CCrazyRadio(string strRadioIdentifier) {
  m_strRadioIdentifier = strRadioIdentifier;
  m_enumPower = P_M18DBM;
  
  m_ctxContext = NULL;
  m_hndlDevice = NULL;
  
  m_bAckReceived = false;
  
  /*int nReturn = */libusb_init(&m_ctxContext);
  
  // Do error checking here.
}

CCrazyRadio::~CCrazyRadio() {
  this->closeDevice();
  
  // TODO(winkler): Free all remaining packets in m_lstLoggingPackets.
  
  if(m_ctxContext) {
    libusb_exit(m_ctxContext);
  }
}

void CCrazyRadio::closeDevice() {
  if(m_hndlDevice) {
    libusb_close(m_hndlDevice);
    libusb_unref_device(m_devDevice);
    
    m_hndlDevice = NULL;
    m_devDevice = NULL;
  }
}

list<libusb_device*> CCrazyRadio::listDevices(int nVendorID, int nProductID) {
  list<libusb_device*> lstDevices;
  ssize_t szCount;
  libusb_device **ptDevices;
  
  szCount = libusb_get_device_list(m_ctxContext, &ptDevices);
  for(unsigned int unI = 0; unI < szCount; unI++) {
    libusb_device *devCurrent = ptDevices[unI];
    libusb_device_descriptor ddDescriptor;
    
    libusb_get_device_descriptor(devCurrent, &ddDescriptor);
    
    if(ddDescriptor.idVendor == nVendorID && ddDescriptor.idProduct == nProductID) {
      libusb_ref_device(devCurrent);
      lstDevices.push_back(devCurrent);
    }
  }
  
  if(szCount > 0) {
    libusb_free_device_list(ptDevices, 1);
  }
  
  return lstDevices;
}

bool CCrazyRadio::openUSBDongle() {
  this->closeDevice();
  list<libusb_device*> lstDevices = this->listDevices(0x1915, 0x7777);
  
  if(lstDevices.size() > 0) {
    // For now, just take the first device. Give it a second to
    // initialize the system permissions.
    sleep(1.0);
    
    libusb_device *devFirst = lstDevices.front();
    int nError = libusb_open(devFirst, &m_hndlDevice);
    
    if(nError == 0) {
      // Opening device OK. Don't free the first device just yet.
      lstDevices.pop_front();
      m_devDevice = devFirst;
    }
    
    for(list<libusb_device*>::iterator itDevice = lstDevices.begin();
	itDevice != lstDevices.end();
	itDevice++) {
      libusb_device *devCurrent = *itDevice;
      
      libusb_unref_device(devCurrent);
    }
    
    return !nError;
  }
  
  return false;
}

bool CCrazyRadio::startRadio() {
  if(this->openUSBDongle()) {
    int nDongleNBR;
    int nRadioChannel;
    int nDataRate;
    char cDataRateType;
    
    if(sscanf(m_strRadioIdentifier.c_str(), "radio://%d/%d/%d%c",
	      &nDongleNBR, &nRadioChannel, &nDataRate,
	      &cDataRateType) != EOF) {
      cout << "Opening radio " << nDongleNBR << "/" << nRadioChannel << "/" << nDataRate << cDataRateType << endl;
      
      stringstream sts;
      sts << nDataRate;
      sts << cDataRateType;
      string strDataRate = sts.str();
      
      // Read device version
      libusb_device_descriptor ddDescriptor;
      libusb_get_device_descriptor(m_devDevice, &ddDescriptor);
      sts.clear();
      sts.str(std::string());
      sts << (ddDescriptor.bcdDevice >> 8);
      sts << ".";
      sts << (ddDescriptor.bcdDevice & 0x0ff);
      sscanf(sts.str().c_str(), "%f", &m_fDeviceVersion);
      
      cout << "Got device version " << m_fDeviceVersion << endl;
      if(m_fDeviceVersion < 0.3) {
	return false;
      }
      
      // Set active configuration to 1
      libusb_set_configuration(m_hndlDevice, 1);
      
      // Claim interface
      if(this->claimInterface(0)) {
	// Set power-up settings for dongle (>= v0.4)
	this->setDataRate("2M");
	this->setChannel(2);
	
	if(m_fDeviceVersion >= 0.4) {
	  this->setContCarrier(false);
	  char cAddress[5];
	  cAddress[0] = 0xe7;
	  cAddress[1] = 0xe7;
	  cAddress[2] = 0xe7;
	  cAddress[3] = 0xe7;
	  cAddress[4] = 0xe7;
	  this->setAddress(cAddress);
	  this->setPower(P_0DBM);
	  this->setARC(3);
	  this->setARDBytes(32);
	}
	
	// Initialize device
	if(m_fDeviceVersion >= 0.4) {
	  this->setARC(10);
	}
	
	this->setChannel(nRadioChannel);
	this->setDataRate(strDataRate);
	
	return true;
      }
    }
  }
  
  return false;
}

CCRTPPacket *CCrazyRadio::writeData(void *vdData, int nLength) {
  CCRTPPacket *crtpPacket = NULL;
  
  int nActuallyWritten;
  int nReturn = libusb_bulk_transfer(m_hndlDevice, (0x01 | LIBUSB_ENDPOINT_OUT), (unsigned char*)vdData, nLength, &nActuallyWritten, 1000);
  
  if(nReturn == 0 && nActuallyWritten == nLength) {
    crtpPacket = this->readACK();
  }
  
  return crtpPacket;
}

bool CCrazyRadio::readData(void *vdData, int &nMaxLength) {
  int nActuallyRead;
  int nReturn = libusb_bulk_transfer(m_hndlDevice, (0x81 | LIBUSB_ENDPOINT_IN), (unsigned char*)vdData, nMaxLength, &nActuallyRead, 50);
  
  if(nReturn == 0) {
    nMaxLength = nActuallyRead;
    
    return true;
  } else {
    switch(nReturn) {
    case LIBUSB_ERROR_TIMEOUT:
      cout << "USB timeout" << endl;
      break;
      
    default:
      break;
    }
  }
  
  return false;
}

bool CCrazyRadio::writeControl(void *vdData, int nLength, uint8_t u8Request, uint16_t u16Value, uint16_t u16Index) {
  int nTimeout = 1000;
  
  /*int nReturn = */libusb_control_transfer(m_hndlDevice, LIBUSB_REQUEST_TYPE_VENDOR, u8Request, u16Value, u16Index, (unsigned char*)vdData, nLength, nTimeout);
  
  // if(nReturn == 0) {
  //   return true;
  // }
  
  // Hack.
  return true;
}

void CCrazyRadio::setARC(int nARC) {
  m_nARC = nARC;
  this->writeControl(NULL, 0, 0x06, nARC, 0);
}

void CCrazyRadio::setChannel(int nChannel) {
  m_nChannel = nChannel;
  this->writeControl(NULL, 0, 0x01, nChannel, 0);
}

void CCrazyRadio::setDataRate(string strDataRate) {
  m_strDataRate = strDataRate;
  int nDataRate = -1;
  
  if(m_strDataRate == "250K") {
    nDataRate = 0;
  } else if(m_strDataRate == "1M") {
    nDataRate = 1;
  } else if(m_strDataRate == "2M") {
    nDataRate = 2;
  }
  
  this->writeControl(NULL, 0, 0x03, nDataRate, 0);
}

void CCrazyRadio::setARDTime(int nARDTime) { // in uSec
  m_nARDTime = nARDTime;
  
  int nT = int((nARDTime / 250) - 1);
  if(nT < 0) {
    nT = 0;
  } else if(nT > 0xf) {
    nT = 0xf;
  }
  
  this->writeControl(NULL, 0, 0x05, nT, 0);
}

void CCrazyRadio::setARDBytes(int nARDBytes) {
  m_nARDBytes = nARDBytes;
  
  this->writeControl(NULL, 0, 0x05, 0x80 | nARDBytes, 0);
}

enum Power CCrazyRadio::power() {
  return m_enumPower;
}

void CCrazyRadio::setPower(enum Power enumPower) {
  m_enumPower = enumPower;

  this->writeControl(NULL, 0, 0x04, enumPower, 0);
}

void CCrazyRadio::setAddress(char *cAddress) {
  m_cAddress = cAddress;
  
  this->writeControl(cAddress, 5, 0x02, 0, 0);
}

void CCrazyRadio::setContCarrier(bool bContCarrier) {
  m_bContCarrier = bContCarrier;
  
  this->writeControl(NULL, 0, 0x20, (bContCarrier ? 1 : 0), 0);
}

bool CCrazyRadio::claimInterface(int nInterface) {
  return libusb_claim_interface(m_hndlDevice, nInterface) == 0;
}

CCRTPPacket *CCrazyRadio::sendPacket(CCRTPPacket *crtpSend, bool bDeleteAfterwards) {
  CCRTPPacket *crtpPacket = NULL;
  
  char *cSendable = crtpSend->sendableData();
  crtpPacket = this->writeData(cSendable, crtpSend->sendableDataLength());
    
  delete[] cSendable;
    
  if(crtpPacket) {
    char *cData = crtpPacket->data();
    int nLength = crtpPacket->dataLength();
    
    if(nLength > 0) {
      short sPort = (cData[0] & 0xf0) >> 4;
      crtpPacket->setPort(sPort);
      short sChannel = cData[0] & 0b00000011;
      crtpPacket->setChannel(sChannel);
      
      switch(sPort) {
      case 0: { // Console
	char cText[nLength];
	memcpy(cText, &cData[1], nLength - 1);
	cText[nLength - 1] = '\0';
	  
	cout << "Console text: " << cText << endl;
      } break;
	
      case 5: { // Logging
	if(crtpPacket->channel() == 2) {
	  CCRTPPacket *crtpLog = new CCRTPPacket(cData, nLength, crtpPacket->channel());
	  crtpLog->setChannel(crtpPacket->channel());
	  crtpLog->setPort(crtpPacket->port());
	  
	  m_lstLoggingPackets.push_back(crtpLog);
	}
      } break;
      }
    }
  }
  
  if(bDeleteAfterwards) {
    delete crtpSend;
  }
  
  return crtpPacket;
}

CCRTPPacket *CCrazyRadio::readACK() {
  CCRTPPacket *crtpPacket = NULL;
  
  int nBufferSize = 64;
  char cBuffer[nBufferSize];
  int nBytesRead = nBufferSize;
  
  if(this->readData(cBuffer, nBytesRead)) {
    if(nBytesRead > 0) {
      // Analyse status byte
      m_bAckReceived = true;//cBuffer[0] & 0x1;
      //bool bPowerDetector = cBuffer[0] & 0x2;
      //int nRetransmissions = cBuffer[0] & 0xf0;
      
      // TODO(winkler): Do internal stuff with the data received here
      // (store current link quality, etc.). For now, ignore it.
      
      crtpPacket = new CCRTPPacket(0);
      
      if(nBytesRead > 1) {
      	crtpPacket->setData(&cBuffer[1], nBytesRead);
      }
    } else {
      m_bAckReceived = false;
    }
  }
  
  return crtpPacket;
}

bool CCrazyRadio::ackReceived() {
  return m_bAckReceived;
}

bool CCrazyRadio::usbOK() {
  libusb_device_descriptor ddDescriptor;
  return (libusb_get_device_descriptor(m_devDevice,
				       &ddDescriptor) == 0);
}

CCRTPPacket *CCrazyRadio::waitForPacket() {
  bool bGoon = true;
  CCRTPPacket *crtpReceived = NULL;
  CCRTPPacket *crtpDummy = new CCRTPPacket(0);
  crtpDummy->setIsPingPacket(true);
  
  while(bGoon) {
    crtpReceived = this->sendPacket(crtpDummy);
    bGoon = (crtpReceived == NULL);
  }
  
  delete crtpDummy;
  return crtpReceived;
}

CCRTPPacket *CCrazyRadio::sendAndReceive(CCRTPPacket *crtpSend, bool bDeleteAfterwards) {
  return this->sendAndReceive(crtpSend, crtpSend->port(), crtpSend->channel(), bDeleteAfterwards);
}

CCRTPPacket *CCrazyRadio::sendAndReceive(CCRTPPacket *crtpSend, int nPort, int nChannel, bool bDeleteAfterwards, int nRetries, int nMicrosecondsWait) {
  bool bGoon = true;
  int nResendCounter = 0;
  CCRTPPacket *crtpReturnvalue = NULL;
  CCRTPPacket *crtpReceived = NULL;
  
  while(bGoon) {
    if(nResendCounter == 0) {
      crtpReceived = this->sendPacket(crtpSend);
      nResendCounter = nRetries;
    } else {
      nResendCounter--;
    }
    
    if(crtpReceived) {
      if(crtpReceived->port() == nPort &&
	 crtpReceived->channel() == nChannel) {
	crtpReturnvalue = crtpReceived;
	bGoon = false;
      }
    }
    
    if(bGoon) {
      if(crtpReceived) {
	delete crtpReceived;
      }
      
      usleep(nMicrosecondsWait);
      crtpReceived = this->waitForPacket();
    }
  }
  
  if(bDeleteAfterwards) {
    delete crtpSend;
  }
  
  return crtpReturnvalue;
}

list<CCRTPPacket*> CCrazyRadio::popLoggingPackets() {
  list<CCRTPPacket*> lstPackets = m_lstLoggingPackets;
  m_lstLoggingPackets.clear();
  
  return lstPackets;
}

bool CCrazyRadio::sendDummyPacket() {
  CCRTPPacket *crtpReceived = NULL;
  CCRTPPacket *crtpDummy = new CCRTPPacket(0);
  crtpDummy->setIsPingPacket(true);
  
  crtpReceived = this->sendPacket(crtpDummy, true);
  if(crtpReceived) {
    delete crtpReceived;
    return true;
  }
  
  return false;
}
