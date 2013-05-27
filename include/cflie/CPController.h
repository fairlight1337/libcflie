#ifndef __C_PCONTROLLER_H__
#define __C_PCONTROLLER_H__


#include "CController.h"


class CPController : public CController {
 private:
  float m_fPGain;

 public:
  CPController();
  ~CPController();
  
  virtual struct DSVelocityControlSignal inputSignalForDesiredPose(struct DSPose dspCurrent, struct DSPose dspDesired);
  
  void setPGain(float fPGain);
  float pGain();
};


#endif /* __C_PCONTROLLER_H__ */
