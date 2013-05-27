#ifndef __C_CRTP_PACKET_H__
#define __C_CRTP_PACKET_H__


#include <iostream>
#include <string>
#include <string.h>

using namespace std;


class CCRTPPacket {
 private:
  char *m_cData;
  int m_nDataLength;
  int m_nPort;
  int m_nChannel;
  
 public:
  CCRTPPacket(int nChannel);
  CCRTPPacket(char *cData, int nDataLength, int nChannel);
  ~CCRTPPacket();
  
  void basicSetup();
  
  void setData(char *cData, int nDataLength);
  char *data();
  int dataLength();
  void clearData();
  
  char *sendableData();
  int sendableDataLength();
  
  void setPort(int nPort);
  int port();

  void setChannel(int nChannel);
  int channel();
};


#endif /* __C_CRTP_PACKET_H__ */
