#include <iostream>

#include "cflie/CCrazyflie.h"

using namespace std;


int main(int argc, char **argv) {
  int nSamplesExit = 100;//2000;
  int nSamples = 0;
  int nThrust = 0;//10001;
  
  string strRadioURI = "radio://0/10/250K";
  
  cout << "Opening radio URI '" << strRadioURI << "'" << endl;
  CCrazyRadio *crRadio = new CCrazyRadio(strRadioURI);
  
  if(crRadio->startRadio()) {
    cout << "Radio started" << endl;
    cout << " - Default Thrust: " << nThrust << endl;
    cout << " - Will exit after: " << nSamplesExit << " cycles" << endl;
    
    CCRTPPacket *crtpReceived = NULL;
    CCrazyflie *cflieCopter = new CCrazyflie(crRadio);
    bool bGoon = true;
      
    cflieCopter->setThrust(nThrust);
      
    while(bGoon) {
      cflieCopter->cycle();
	
      nSamples++;
      if(nSamples == nSamplesExit) {
	bGoon = false;
      }
    }
      
    delete cflieCopter;
  } else {
    cerr << "Radio dongle could not be opened. Did you plug it in?" << endl;
  }
  
  cout << "Cleaning up" << endl;
  delete crRadio;
  
  return 0;
}
