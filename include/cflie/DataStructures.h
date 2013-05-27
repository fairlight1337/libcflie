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
  struct DSOrientation dsvAngular;
};


struct DSVelocityControlSignal {
  int nThrust;
  struct DSOrientation dsoAngular;
};


#endif /* __DATA_STRUCTURES_H__ */
