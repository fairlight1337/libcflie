#ifndef __C_CRAZYFLIE_H__
#define __C_CRAZYFLIE_H__


#include <iostream>
#include <sstream>

#include "CCrazyRadio.h"

using namespace std;


class CCrazyflie {
 private:
  CCrazyRadio *m_crRadio;
  int m_nLastRequestedVariableIndex;
  int m_nThrust;

 public:
  CCrazyflie(CCrazyRadio *crRadio);
  ~CCrazyflie();
  
  bool sendSetpoint(float fRoll, float fPitch, float fYaw, short sThrust);
  void updateTOC();
  void populateTOCElement(int nIndex);
  void populateNextTOCElement();

  void populateLOGElement(int nIndex);
  void populateNextLOGElement();
  
  void updateLogTOC();
  
  void setThrust(int nThrust);
  int thrust();

  void cycle();
};


#endif /* __C_CRAZYFLIE_H__ */
