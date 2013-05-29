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

#ifndef __C_CONTROLLER_H__
#define __C_CONTROLLER_H__


#include <math.h>

#include "DataStructures.h"


/*! \brief Basic controller superclass supplying variables and
    functions valid for all controllers
  
  All controllers need the flag whether they should ignore or take
  into account the copter yaw while controlling it's state. Also, The
  virtual function for generating a linear and angular twist for a
  desired pose is defined here. Other than that, the class does not do
  anything and only serves as a superclass for controllers such as the
  P-controller (CPController). */
class CController {
 private:
  /*! \brief Internal flag denoting whether yaw control is ignored */
  bool m_bIgnoresYaw;

 public:
  /*! \brief Constructor for the basic controller class.
    
    Initializes internal flags. */
  CController();
  /*! \brief Destructor for the basic controller class. */
  ~CController();
  
  /*! \brief Sets whether the yaw control is ignored or not
    
    When setting this flag, the controller (and it's subclasses)
    ignores control of the copter's yaw.
    
    \param bIgnoresYaw Boolean value denoting whether or not the yaw
    control is ignored when calculating the control signal */
  void setIgnoresYaw(bool bIgnoresYaw);
  /*! \brief Returns whether yaw control is ignored
    
    Returns the internal state that denoted whether the copter's yaw
    is being controlled or not.
  
    \return Boolean value that denotes the current ignorance setting
    of copter yaw control */
  bool ignoresYaw();
  
  /*! \brief Calculates the current controller output signal
    
    The control signal depends on the current pose of the copter, as
    well, as the desired set point given as DSControlSetPoint. For
    more elaborate controllers, it might also depend on an internal
    controller state. The return value is a DSVelocityControlSignal in
    which the angular twist as well as the thrust to be sent to the
    copter's internal controller is stored.
    
    \param dspCurrent The current pose of the copter. This is supplied
    by an instance of the governing class, i.e. CCrazyflie \param
    cspDesired Denoted the desired set point, consisting of R^3
    position and optional yaw (depending on the ignoresYaw() value)
    
    \return Returns the controller output signal, containing absolute
    roll, pitch, yaw and thrust values to be sent to the copter. */
  virtual struct DSVelocityControlSignal inputSignalForDesiredPosition(struct DSPose dspCurrent, struct DSControlSetPoint cspDesired);
};


#endif /* __C_CONTROLLER_H__ */
