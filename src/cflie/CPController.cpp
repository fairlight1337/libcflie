// Copyright (c) 2013, Jan Winkler <winkler@cs.uni-bremen.de>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Universität Bremen nor the names of its
//       contributors may be used to endorse or promote products derived from
//       this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

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
  
  if(!this->ignoresYaw()) {
    // Only do this if we actually care about the yaw.
    dvcsResult.dsoAngular.fYaw = m_fPGain * (cspDesired.fYaw - dspCurrent.dsoOrientation.fYaw);
  }
  
  return dvcsResult;
}

void CPController::setPGain(float fPGain) {
  m_fPGain = fPGain;
}

float CPController::pGain() {
  return m_fPGain;
}
