#include "cflie/CPController.h"


CPController::CPController() {
  this->setPGain(0.5f);
}

CPController::~CPController() {
}

struct DSVelocityControlSignal CPController::inputSignalForDesiredPose(struct DSPose dspCurrent, struct DSControlSetPoint cspDesired) {
  struct DSVelocityControlSignal dvcsResult;
  
  float fDistanceToGoXY = sqrt((cspDesired.dsvPosition.fX - dspCurrent.dsvPosition.fX) * (cspDesired.dsvPosition.fX - dspCurrent.dsvPosition.fX) +
			       (cspDesired.dsvPosition.fY - dspCurrent.dsvPosition.fY) * (cspDesired.dsvPosition.fY - dspCurrent.dsvPosition.fY) +
			       (cspDesired.dsvPosition.fZ - dspCurrent.dsvPosition.fZ) * (cspDesired.dsvPosition.fZ - dspCurrent.dsvPosition.fZ));
  float fDifferenceZ = cspDesired.dsvPosition.fZ - dspCurrent.dsvPosition.fZ;
  
  // Upwards driven thrust motion
  dvcsResult.nThrust = 30000 + m_fPGain * 20000 * fDifferenceZ;
  
  // Angle driven sideways motion
  float fAngle = atan2(cspDesired.dsvPosition.fX - dspCurrent.dsvPosition.fX,
		       cspDesired.dsvPosition.fY - dspCurrent.dsvPosition.fY) + dspCurrent.dsoOrientation.fYaw;
  
  dvcsResult.dsoAngular.fRoll = m_fPGain * fDistanceToGoXY * sin(fAngle);
  dvcsResult.dsoAngular.fPitch = m_fPGain * fDistanceToGoXY * cos(fAngle);
  
  dvcsResult.dsoAngular.fYaw = m_fPGain * (dspCurrent.dsoOrientation.fYaw - cspDesired.fYaw);
  
  return dvcsResult;
}

void CPController::setPGain(float fPGain) {
  m_fPGain = fPGain;
}

float CPController::pGain() {
  return m_fPGain;
}
