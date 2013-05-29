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

#ifndef __C_PCONTROLLER_H__
#define __C_PCONTROLLER_H__


#include "CController.h"


/*! \brief Simple P gain based controller class
  
  Simple controller that calculates the control signal (copter
  angles/thrust) based on the linear difference between the current
  pose and the set point. */
class CPController : public CController {
 private:
  /*! \brief Stores the proportional gain for the controller */
  float m_fPGain;

 public:
  CPController();
  ~CPController();
  
  virtual struct DSVelocityControlSignal inputSignalForDesiredPosition(struct DSPose dspCurrent, struct DSControlSetPoint cspDesired);

  /*! \brief Sets the proportional gain constant for the controller
    
    This influences the 'aggressiveness' of the controller.
    
    Values > 1 will render the controller unstable due to
    over-proportional overshoot, < 1 will invert it's purpose
    (i.e. move away from the set point and also become
    unstable). Setting it to zero switches off the control ability.
    
    Valid values are between 0 < p < 1. The exact value depends on the
    purpose and should be tried out. Start at around p = 0.5.
    
    \param fPGain The proportional gain constant to be set for the
    controller */
  void setPGain(float fPGain);
  /*! \brief Returns the proportional gain constant for the controller
    
    Returns the internally stored proportional gain value for the
    P-gain based control algorithm.
    
    \return pGain Returns the proportional gain constant */
  float pGain();
};


#endif /* __C_PCONTROLLER_H__ */
