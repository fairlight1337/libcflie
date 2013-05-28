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

#include "cflie/CController.h"


/*! \brief Constructor for the basic controller superclass

  Basic superclass for controllers. It does not generate control
  signals (besides the identity control signal) and only serves as a
  template for subclasses. */
CController::CController() {
  this->setIgnoresYaw(false);
}

/*! \brief Destructor for the basic controller superclass */
CController::~CController() {
}

/*! \brief Sets whether the yaw is taken into account for control
  
  When set to 'true', the yaw is not part of the control
  variables. This means that the controller gains one degree of
  freedom (the yaw of the copter) and dies not actively control the
  yaw. When set to 'false', no yaw value is generated. This also means
  that the yaw might drift.
  
  \param bIgnoresYaw Boolean value denoting whether or not the yaw should be ignored while controlling the copter. */
void CController::setIgnoresYaw(bool bIgnoresYaw) {
  m_bIgnoresYaw = bIgnoresYaw;
}

/*! \brief Returns the current handling status of the copter yaw control
  
  Returns whether the yaw of the copter is taken into account by control as set in setIgnoreYaw(bool bIgnoresYaw).
  
  \return Boolean value denoting whether or not the yaw should be ignored while controlling the copter. */
bool CController::ignoresYaw() {
  return m_bIgnoresYaw;
}

struct DSVelocityControlSignal CController::inputSignalForDesiredPose(struct DSPose dspCurrent, struct DSControlSetPoint cspDesired) {
  struct DSVelocityControlSignal dvcsZero;
  
  // NOTE(winkler): Do calculation in the *actual* controllers. This
  // one will only return zero values.
  
  return dvcsZero;
}
