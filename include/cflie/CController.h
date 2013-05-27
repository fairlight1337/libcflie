#ifndef __C_CONTROLLER_H__
#define __C_CONTROLLER_H__


#include <math.h>

#include "DataStructures.h"


class CController {
 private:
 public:
  CController();
  ~CController();
  
  virtual struct DSVelocityControlSignal inputSignalForDesiredPose(struct DSPose dspCurrent, struct DSControlSetPoint cspDesired);
};


#endif /* __C_CONTROLLER_H__ */
