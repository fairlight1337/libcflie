#ifndef __C_TOC_H__
#define __C_TOC_H__


#include <list>
#include <string>
#include <stdlib.h>

#include "CCrazyRadio.h"
#include "CCRTPPacket.h"

using namespace std;


/*! \brief Storage element for logged variable identities */
struct TOCElement {
  /*! \brief The numerical ID of the log element on the copter's
      internal table */
  int nID;
  /*! \brief The (ref) type of the log element */
  int nType;
  /*! \brief The string group name of the log element */
  string strGroup;
  /*! \brief The string identifier of the log element */
  string strIdentifier;
  bool bIsLogging;
  double dValue;
};


struct LoggingBlock {
  string strName;
  int nID;
  double dFrequency;
  list<int> lstElementIDs;
};


class CTOC {
 private:
  int m_nPort;
  CCrazyRadio *m_crRadio;
  int m_nItemCount;
  list<struct TOCElement> m_lstTOCElements;
  list<struct LoggingBlock> m_lstLoggingBlocks;
  
  bool requestInitialItem();
  bool requestItem(int nID, bool bInitial);
  bool requestItem(int nID);
  bool processItem(CCRTPPacket *crtpItem);
  
  CCRTPPacket *sendAndReceive(CCRTPPacket *crtpSend, int nChannel);
  
 public:
  CTOC(CCrazyRadio *crRadio, int nPort);
  ~CTOC();
  
  bool sendTOCPointerReset();
  bool requestMetaData();
  bool requestItems();
  
  struct TOCElement elementForName(string strName, bool &bFound);
  struct TOCElement elementForID(int nID, bool &bFound);
  int idForName(string strName);
  int typeForName(string strName);
  
  // For loggable variables only
  bool registerLoggingBlock(string strName, double dFrequency);
  bool unregisterLoggingBlock(string strName);
  struct LoggingBlock loggingBlockForName(string strName, bool &bFound);
  struct LoggingBlock loggingBlockForID(int nID, bool &bFound);
  
  bool startLogging(string strName, string strBlockName);
  bool stopLogging(string strName);
  bool isLogging(string strName);
  
  double doubleValue(string strName);
  
  bool enableLogging(string strBlockName);
  
  void processPackets(list<CCRTPPacket*> lstPackets);
  
  int elementIDinBlock(int nBlockID, int nElementIndex);
  bool setFloatValueForElementID(int nElementID, float fValue);
  bool addElementToBlock(int nBlockID, int nElementID);
  bool unregisterLoggingBlockID(int nID);
};


#endif /* __C_TOC_H__ */
