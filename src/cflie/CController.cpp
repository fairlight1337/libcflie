#include "cflie/CController.h"


CController::CController() {
  this->setIgnoresYaw(false);
}

CController::~CController() {
}

void CController::setIgnoresYaw(bool bIgnoresYaw) {
  m_bIgnoresYaw = bIgnoresYaw;
}

bool CController::ignoresYaw() {
  return m_bIgnoresYaw;
}

struct DSVelocityControlSignal CController::inputSignalForDesiredPose(struct DSPose dspCurrent, struct DSControlSetPoint cspDesired) {
  struct DSVelocityControlSignal dvcsZero;
  
  // NOTE(winkler): Do calculation in the *actual* controllers. This
  // one will only return zero values.
  
  return dvcsZero;
}
