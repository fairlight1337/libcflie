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
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glfw.h>

#include <cflie/CCrazyflie.h>

using namespace std;


bool g_bGoon;


void drawGL(float fX, float fY, float fZ) {
  glLoadIdentity();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glMatrixMode(GL_MODELVIEW);

  /* Move down the z-axis. */
  glTranslatef(0.0, 0.0, -5.0);
  
  /* Rotate. */
  glRotatef(fX - 45.0f, 1.0, 0.0, 0.0);
  glRotatef(fY, 0.0, 1.0, 0.0);
  glRotatef(fZ, 0.0, 0.0, 1.0);
  
  float fQuadWidth = 2;
  float fQuadHeight = fQuadWidth;
  
  glBegin(GL_QUADS); {
    glColor3f(1, 1, 1);
    
    glVertex2f(fQuadWidth / 2, fQuadHeight / 2);
    glVertex2f(fQuadWidth / 2, -fQuadHeight / 2);
    glVertex2f(-fQuadWidth / 2, -fQuadHeight / 2);
    glVertex2f(-fQuadWidth / 2, fQuadHeight / 2);
  }
  glEnd();
  
  glfwSwapBuffers();
}

int main(int argc, char **argv) {
  CCrazyRadio *crRadio = new CCrazyRadio("radio://0/10/250K");
  
  if(crRadio->startRadio()) {
    CCrazyflie *cflieCopter = new CCrazyflie(crRadio);
    cflieCopter->setSendSetpoints(true);
    cflieCopter->setThrust(0);
    
    if(glfwInit() == GL_TRUE) {
      g_bGoon = true;
      
      int nWidth = 800, nHeight = 600;
      int nBitsPerComponent = 8, nDepthBits = 0, nStencilBits = 0;
      int nOpenGLMode = GLFW_WINDOW;
      
      if(glfwOpenWindow(nWidth, nHeight,
			nBitsPerComponent, nBitsPerComponent, nBitsPerComponent, nBitsPerComponent, nDepthBits, nStencilBits, nOpenGLMode)) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float fAspectRatio = ((float)nHeight) / ((float)nWidth);
	glFrustum(.5, -.5, -.5 * fAspectRatio, .5 * fAspectRatio, 1, 50);
	glMatrixMode(GL_MODELVIEW);
	
	cout << "Running, exit with 'ESC'." << endl;
	while(g_bGoon) {
	  if(cflieCopter->cycle()) {
	    drawGL(cflieCopter->roll(),
		   cflieCopter->pitch(),
		   cflieCopter->yaw());
	    
	    if(glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS) {
	      cflieCopter->setThrust(0);
	      g_bGoon = false;
	    } else {
	      if(glfwGetKey(GLFW_KEY_SPACE) == GLFW_PRESS) {
		cflieCopter->setThrust(45000);
	      } else {
		cflieCopter->setThrust(30000);
	      }
	      
	      double dRoll = 0;
	      double dPitch = 0;
	      double dYaw = cflieCopter->yaw();
	      
	      if(glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS) {
		dRoll = 20.0f;//dYaw += 20.0f;
	      } else if(glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) {
		dRoll = -20.0f;//dYaw -= 20.0f;
	      }
	      
	      if(glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS) {
		dPitch = 20.0f;
	      } else if(glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS) {
		dPitch = -20.0f;
	      }
	      
	      cflieCopter->setRoll(dRoll);
	      cflieCopter->setPitch(dPitch);
	      cflieCopter->setYaw(dYaw);
	    }
	  } else {
	    g_bGoon = false;
	  }
	}
	
	glfwCloseWindow();
      }
      
      glfwTerminate();
    }
    
    delete cflieCopter;
  } else {
    cerr << "Radio could not be started." << endl;
  }
  
  delete crRadio;
  
  return 0;
}
