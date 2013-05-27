#include "cflie/CController.h"


CController::CController() {
}

CController::~CController() {
}

struct DSVelocityControlSignal CController::inputSignalForDesiredPose(struct DSPose dspCurrent, struct DSControlSetPoint cspDesired) {
  struct DSVelocityControlSignal dvcsZero;
  
  // NOTE(winkler): Do calculation in the *actual* controllers. This
  // one will only return zero values.
  
  return dvcsZero;
}
