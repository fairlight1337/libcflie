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

#ifndef __DATA_STRUCTURES_H__
#define __DATA_STRUCTURES_H__


/*! \brief Struct describing a R^3 vector
  
  Struct that holds information about extends in three directions,
  namely X, Y, and Z.
  
  \param fX Extent in X direction
  \param fY Extent in Y direction
  \param fZ Extent in Z direction */
struct DSVector {
  float fX;
  float fY;
  float fZ;
};


/*! \brief Struct describing a R^3 orientation
  
  Struct that holds information about orientation in the R^3, namely
  roll, pitch, and yaw.
  
  \param fRoll Rotation around the X axis
  \param fPitch Rotation around the Y axis
  \param fYaw Rotation around the Z axis */
struct DSOrientation {
  float fRoll;
  float fPitch;
  float fYaw;
};


/*! \brief Struct describing position and orientation
  
  Struct that holds information about position and orientation in the
  R^3 space.
  
  \param dsvPosition Position vector in R^3
  \param dsoOrientation Orientation vector in R^3 */
struct DSPose {
  struct DSVector dsvPosition;
  struct DSOrientation dsoOrientation;
};


/*! \brief Struct describing linear and angular twist
  
  Struct that holds information about twist in linear and angular
  respect, describing how the `velocity' in both currently is.
  
  \param dsvLinear Linear velocity in R^3
  \param dsoAngular Angular velocity R^3 */
struct DSTwist {
  struct DSVector dsvLinear;
  struct DSOrientation dsoAngular;
};


/*! \brief Struct holding a controller output signal
  
  As the copter internal controller takes fixed angles and a thrust
  value as input, the controllers in the client library generate the
  corresponding values and store it in this struct.
  
  \param nThrust The amount of thrust to give to the copter controller
  \param dsoAngular The fixed angles to give to the copter controller */
struct DSVelocityControlSignal {
  int nThrust;
  struct DSOrientation dsoAngular;
};


/*! \brief Set point struct for the client library controller(s)
  
  In order to go to a certain position using the client library
  controller(s), a set point must be supplied. It is defined using
  this struct. Also, the yaw can be defined here (CController and it's
  subclasses only take it into account when setIgnoresYaw(bool
  bControllerIgnoresYaw) in the CCrazyflie class is set to 'false').
  
  \param dsvPosition The set point position to reach (control goal)
  \param dsoAngular The yaw to achieve (control goal, if option is enabled) */
struct DSControlSetPoint {
  DSVector dsvPosition;
  float fYaw;
};


#endif /* __DATA_STRUCTURES_H__ */
