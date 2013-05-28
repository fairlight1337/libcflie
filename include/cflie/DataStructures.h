#ifndef __DATA_STRUCTURES_H__
#define __DATA_STRUCTURES_H__


struct DSVector {
  float fX;
  float fY;
  float fZ;
};


struct DSOrientation {
  float fRoll;
  float fPitch;
  float fYaw;
};


struct DSPose {
  struct DSVector dsvPosition;
  struct DSOrientation dsoOrientation;
};


struct DSTwist {
  struct DSVector dsvLinear;
  struct DSOrientation dsoAngular;
};


struct DSVelocityControlSignal {
  int nThrust;
  struct DSOrientation dsoAngular;
};


struct DSControlSetPoint {
  DSVector dsvPosition;
  float fYaw;
};


#endif /* __DATA_STRUCTURES_H__ */
