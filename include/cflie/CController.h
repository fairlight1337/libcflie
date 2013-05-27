#ifndef __C_CONTROLLER_H__
#define __C_CONTROLLER_H__


#include "DataStructures.h"


class CController {
 private:
 public:
  CController();
  ~CController();
  
  virtual struct DSVelocityControlSignal inputSignalForDesiredPose(struct DSPose dspCurrent, struct DSPose dspDesired);
};


#endif /* __C_CONTROLLER_H__ */
