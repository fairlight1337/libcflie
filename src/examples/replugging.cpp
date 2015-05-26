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


/* \author Jan Winkler */


// System
#include <iostream>
#include <csignal>

// libcflie
#include <cflie/CCrazyflie.h>


bool g_bGoon;


void interruptionHandler(int dummy = 0) {
  g_bGoon = false;
}

int main(int argc, char **argv) {
  std::signal(SIGINT, interruptionHandler);

  int nReturnvalue = 0;
  int nThrust = 10001;

  std::string strRadioURI = "radio://0/10/250K";

  std::cout << "Opening radio URI '" << strRadioURI << "'" << std::endl;
  CCrazyRadio *crRadio = new CCrazyRadio(strRadioURI);

  g_bGoon = true;
  bool bDongleConnected = false;
  bool bDongleNotConnectedNotified = false;

  struct timespec tmWait;
  tmWait.tv_sec = 0;
  tmWait.tv_nsec = 500000000;

  while(g_bGoon) {
    // Is the dongle connected? If not, try to connect it.
    if(!bDongleConnected) {
      while(!crRadio->startRadio() && g_bGoon) {
	if(!bDongleNotConnectedNotified) {
	  std::cout << "Waiting for dongle." << std::endl;
	  bDongleNotConnectedNotified = true;
	}

	nanosleep(&tmWait, NULL);
      }

      if(g_bGoon) {
	std::cout << "Dongle connected, radio started." << std::endl;
      }
    }

    bool bRangeStateChangedNotified = false;
    bool bCopterWasInRange = false;

    if(g_bGoon) {
      bDongleNotConnectedNotified = false;
      bDongleConnected = true;

      CCrazyflie *cflieCopter = new CCrazyflie(crRadio);
      cflieCopter->setSendSetpoints(true);
      cflieCopter->setThrust(nThrust);

      while(g_bGoon && bDongleConnected) {
	if(cflieCopter->cycle()) {
	  if(cflieCopter->copterInRange()) {
	    if(!bCopterWasInRange || !bRangeStateChangedNotified) {
	      // Event triggered when the copter first comes in range.
	      std::cout << "In range" << std::endl;

	      bCopterWasInRange = true;
	      bRangeStateChangedNotified = true;
	    }

	    // Loop body for when the copter is in range continuously
	  } else {
	    if(bCopterWasInRange || !bRangeStateChangedNotified) {
	      // Event triggered when the copter leaves the range.
	      std::cout << "Not in range" << std::endl;

	      bCopterWasInRange = false;
	      bRangeStateChangedNotified = true;
	    }

	    // Loop body for when the copter is not in range
	  }
	} else {
	  std::cerr << "Connection to radio dongle lost." << std::endl;

	  bDongleConnected = false;
	}
      }

      if(!g_bGoon) {
	// NOTE(winkler): Here would be the perfect place to initiate a
	// landing sequence (i.e. ramping down the altitude of the
	// copter). Right now, this is a dummy branch.
      }

      delete cflieCopter;
    }
  }

  std::cout << "Cleaning up" << std::endl;
  delete crRadio;

  return nReturnvalue;
}
