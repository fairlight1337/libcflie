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

#ifndef __C_CRAZY_RADIO_H__
#define __C_CRAZY_RADIO_H__


#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <string.h>

#include "CCRTPPacket.h"

using namespace std;


struct TOCElement {
  int nIndex;
  string strGroup;
  string strIdentifier;
};

struct LOGElement {
  int nID;
  int nType;
  string strGroup;
  string strIdentifier;
};


class CCrazyRadio {
private:
  // Variables
  string m_strRadioIdentifier;
  libusb_context *m_ctxContext;
  libusb_device *m_devDevice;
  libusb_device_handle *m_hndlDevice;
  int m_nARC;
  int m_nChannel;
  string m_strDataRate;
  int m_nARDTime;
  int m_nARDBytes;
  int m_nPower;
  char *m_cAddress;
  int m_bContCarrier;
  float m_fDeviceVersion;
  int m_nParameterCount;
  bool m_bUpdatesParameterCount;
  list<struct TOCElement> m_lstTOCElements;
  bool m_bPopulatesTOCCache;
  int m_nLoggingVariableCount;
  bool m_bUpdatesLogParameterCount;
  int m_nNextLogVarID;
  list<struct LOGElement> m_lstLOGElements;
  bool m_bPopulatesLOGCache;
  bool m_bLogelementPopulated;
  bool m_bAckReceived;
  
  // Functions
  list<libusb_device*> listDevices(int nVendorID, int nProductID);
  bool openUSBDongle();

public:
  CCrazyRadio(string strRadioIdentifier);
  ~CCrazyRadio();
  
  bool startRadio();
  
  void setARC(int nARC);
  void setChannel(int nChannel);
  void setDataRate(string strDataRate);
  void setARDBytes(int nARDBytes);
  void setARDTime(int nARDTime);
  void setPower(int nPower);
  void setAddress(char *cAddress);
  void setContCarrier(bool bContCarrier);
  
  CCRTPPacket *writeData(void *vdData, int nLength);
  bool readData(void *vdData, int &nMaxLength);
  
  CCRTPPacket *sendPacket(CCRTPPacket *crtpSend);
  CCRTPPacket *readPacket();
  CCRTPPacket *readACK();
  
  bool writeControl(void *vdData, int nLength, uint8_t u8Request, uint16_t u16Value, uint16_t u16Index);
  
  bool claimInterface(int nInterface);
  
  void setParameterCount(int nParameterCount);
  int parameterCount();
  
  bool updatesParameterCount();
  void setUpdatesParameterCount(bool bUpdatesParameterCount);
  
  bool populatesTOCCache();
  void setPopulatesTOCCache(bool bPopulatesTOCCache);
  
  void setLoggingVariableCount(int nLoggingVariableCount);
  int loggingVariableCount();
  
  void setUpdatesLogParameterCount(bool bUpdatesLogParameterCount);
  bool updatesLogParameterCount();
  int logParameterCount();
  
  void setPopulatesLOGCache(bool bPopulatesLOGCache);
  bool populatesLOGCache();
  bool logElementPopulated();
  void setLogElementPopulated(bool bLogElementPopulated);
  
  int nextLogVarID();
  int countLOGElements();
  
  bool ackReceived();
  bool usbOK();
};


#endif /* __C_CRAZY_RADIO_H__ */
