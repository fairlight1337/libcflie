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
};


#endif /* __C_CRAZY_RADIO_H__ */
