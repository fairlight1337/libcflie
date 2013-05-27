#include "cflie/CPController.h"


CPController::CPController() {
  this->setPGain(0.5f);
}

CPController::~CPController() {
}

struct DSTwist CPController::twistForDesiredPose(struct DSPose dspCurrent, struct DSPose dspDesired) {
  struct DSTwist dstResult;
  
  dstResult.dsvLinear.fX = m_fPGain * (dspDesired.dsvPosition.fX - dspCurrent.dsvPosition.fX);
  dstResult.dsvLinear.fY = m_fPGain * (dspDesired.dsvPosition.fY - dspCurrent.dsvPosition.fY);
  dstResult.dsvLinear.fZ = m_fPGain * (dspDesired.dsvPosition.fZ - dspCurrent.dsvPosition.fZ);
  
  return dstResult;
}

void CPController::setPGain(float fPGain) {
  m_fPGain = fPGain;
}

float CPController::pGain() {
  return m_fPGain;
}
