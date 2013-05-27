#include "cflie/CController.h"


CController::CController() {
}

CController::~CController() {
}

struct DSTwist CController::twistForDesiredPose(struct DSPose dspCurrent, struct DSPose dspDesired) {
  struct DSTwist dstZero;
  
  // NOTE(winkler): Do calculation in the *actual* controllers. This
  // one will only return zero values.
  
  return dstZero;
}
