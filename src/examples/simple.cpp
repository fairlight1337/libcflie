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

#include <iostream>

#include <cflie/CCrazyflie.h>

using namespace std;


int main(int argc, char **argv) {
  CCrazyRadio *crRadio = new CCrazyRadio("radio://0/10/250K");
  
  if(crRadio->startRadio()) {
    CCrazyflie *cflieCopter = new CCrazyflie(crRadio);
    cflieCopter->setThrust(10001);
    
    // Enable sending the setpoints. This can be used to temporarily
    // stop updating the internal controller setpoints and instead
    // sending dummy packets (to keep the connection alive).
    cflieCopter->setSendSetpoints(true);
    
    while(cflieCopter->cycle()) {
      // Main loop. Currently empty.
      
      /* Examples to set thrust, and RPY:
	 
	 // Range: 10001 - (approx.) 60000
	 cflieCopter->setThrust(10001);
	 
	 // All in degrees. R/P shouldn't be over 45 degree (it goes
	 // sidewards really fast!). R/P/Y are all from -180.0deg to 180.0deg.
	 cflieCopter->setRoll(20);
	 cflieCopter->setPitch(15);
	 cflieCopter->setYaw(140); */
      
      // Important note: When quitting the program, please don't just
      // SIGINT (i.e. CTRL-C) it. The CCrazyflie class instance
      // cflieCopter must be deleted in order to call the destructor
      // which stops logging on the device. If you fail to do this
      // when quitting your program, your copter will experience some
      // kind of buffer overflow (because of a lot of logging messages
      // summing up without being collected) and you will have to
      // restart it manually. This is not being done in this
      // particular example. You have been warned.
      
      // Other than that, this example covers pretty much everything
      // basic you will need for controlling the copter.
    }
    
    delete cflieCopter;
  } else {
    cerr << "Could not connect to dongle. Did you plug it in?" << endl;
  }
  
  delete crRadio;
  return 0;
}
