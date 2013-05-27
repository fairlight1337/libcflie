#include "cflie/CPController.h"


CPController::CPController() {
  this->setPGain(0.5f);
}

CPController::~CPController() {
}

struct DSVelocityControlSignal CPController::inputSignalForDesiredPose(struct DSPose dspCurrent, struct DSPose dspDesired) {
  struct DSVelocityControlSignal dvcsResult;
  
  // dvcsResult.dsvLinear.fX = m_fPGain * (dspDesired.dsvPosition.fX - dspCurrent.dsvPosition.fX);
  // dstResult.dsvLinear.fY = m_fPGain * (dspDesired.dsvPosition.fY - dspCurrent.dsvPosition.fY);
  // dstResult.dsvLinear.fZ = m_fPGain * (dspDesired.dsvPosition.fZ - dspCurrent.dsvPosition.fZ);
  
  return dvcsResult;
}

void CPController::setPGain(float fPGain) {
  m_fPGain = fPGain;
}

float CPController::pGain() {
  return m_fPGain;
}
