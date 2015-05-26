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


#ifndef __C_CRAZY_RADIO_H__
#define __C_CRAZY_RADIO_H__


// System
#include <list>
#include <string>
#include <cstdio>
#include <cstring>
#include <libusb-1.0/libusb.h>
#include <unistd.h>
#include <iostream>
#include <sstream>

// Private
#include "CCRTPPacket.h"


/*! \brief Power levels to configure the radio dongle with */
enum Power {
  /*! \brief Power at -18dbm */
  P_M18DBM = 0,
  /*! \brief Power at -12dbm */
  P_M12DBM = 1,
  /*! \brief Power at -6dbm */
  P_M6DBM = 2,
  /*! \brief Power at 0dbm */
  P_0DBM = 3
};


/*! \brief Communication class to connect to and communicate via the
    CrazyRadio USB dongle.

    The class is capable of finding the CrazyRadio USB dongle on the
    host computer, open and maintain a connection, and send/receive
    data when communicating with the Crazyflie Nano copter using the
    Crazy Radio Transfer Protocol as defined by Bitcraze. */
class CCrazyRadio {
private:
  // Variables
  /*! \brief The radio URI as supplied when initializing the class
      instance */
  std::string m_strRadioIdentifier;
  /*! \brief The current USB context as supplied by libusb */
  libusb_context *m_ctxContext;
  libusb_device *m_devDevice;
  libusb_device_handle *m_hndlDevice;
  int m_nARC;
  int m_nChannel;
  std::string m_strDataRate;
  int m_nARDTime;
  int m_nARDBytes;
  enum Power m_enumPower;
  char *m_cAddress;
  int m_bContCarrier;
  float m_fDeviceVersion;
  bool m_bAckReceived;
  std::list<CCRTPPacket*> m_lstLoggingPackets;

  // Functions
  std::list<libusb_device*> listDevices(int nVendorID, int nProductID);
  bool openUSBDongle();
  bool claimInterface(int nInterface);
  void closeDevice();

  CCRTPPacket *readACK();

  CCRTPPacket *writeData(void *vdData, int nLength);
  bool writeControl(void *vdData, int nLength, uint8_t u8Request, uint16_t u16Value, uint16_t u16Index);
  bool readData(void *vdData, int &nMaxLength);

  void setARC(int nARC);
  void setChannel(int nChannel);
  void setDataRate(std::string strDataRate);
  void setARDBytes(int nARDBytes);
  void setARDTime(int nARDTime);
  void setAddress(char *cAddress);
  void setContCarrier(bool bContCarrier);

public:
  /*! \brief Constructor for the radio communication class

    \param strRadioIdentifier URI for the radio to be opened,
    e.g. "radio://<dongle-no>/<channel-no>/<datarate>". */
  CCrazyRadio(std::string strRadioIdentifier);
  /*! \brief Destructor for the radio communication class */
  ~CCrazyRadio();

  /*! \brief Function to start the radio communication

    The first available USB dongle will be opened and claimed for
    communication. The connection will be maintained and used to
    communicate with a Crazyflie Nano quadcopter in range.

    \return Returns 'true' if the connection could successfully be
    made and 'false' if no dongle could be found (or any other
    USB-related error came up - this is not handled here). */
  bool startRadio();

  /*! \brief Returns the current setting for power usage by the USB
      dongle

    \return Value denoting the current power settings reserved for
    communication */
  enum Power power();
  /*! \brief Set the power level to be used for communication purposes

    \param enumPower The level of power that is being used for
    communication. The integer value maps to one of the entries of the
    Power enum. */
  void setPower(enum Power enumPower);

  /*! \brief Sends the given packet's payload to the copter

    \param crtpSend The packet which supplied header and payload
    information to send to the copter */
  CCRTPPacket *sendPacket(CCRTPPacket *crtpSend, bool bDeleteAfterwards = false);

  /*! \brief Sends the given packet and waits for a reply.

    Internally, this function calls the more elaborate
    sendAndReceive() function supplying parameters for retrying and
    waiting. Convenience function signature.

    \param crtpSend Packet to send
    \param bDeleteAfterwards Whether or not the packet to send is
    deleted internally after sending it

    \return Packet containing the reply or NULL if no reply was
    received (after retrying). */
  CCRTPPacket *sendAndReceive(CCRTPPacket *crtpSend, bool bDeleteAfterwards = false);

  /*! \brief Sends the given packet and waits for a reply.

    Sends out the CCRTPPacket instance denoted by crtpSend on the
    given port and channel. Retries a number of times and waits
    between each retry whether or not an answer was received (in this
    case, dummy packets are sent in order to receive replies).

    \param crtpSend Packet to send

    \param nPort Port number on which to send this packet (and where
    to wait for the reply)
    \param nChannel Channel number on which to send this packet (and
    where to wait for the reply)
    \param bDeletAfterwards Whether or not the packet to send is
    deleted internally after sending it
    \param nRetries Number of retries (re-sending) before giving up on
    an answer
    \param nMicrosecondsWait Microseconds to wait between two re-sends

    \return Packet containing the reply or NULL if no reply was
    received (after retrying). */
  CCRTPPacket *sendAndReceive(CCRTPPacket *crtpSend, int nPort, int nChannel, bool bDeleteAfterwards = true, int nRetries = 10, int nMicrosecondsWait = 100);

  /*! \brief Sends out an empty dummy packet

    Only contains the payload `0xff`, as used for empty packet
    requests. Mostly used for waiting or keepalive.

    \return Boolean value denoting whether sending the dummy packet
    worked or not. */
  bool sendDummyPacket();

  /*! \brief Waits for the next non-empty packet.

    Sends out dummy packets until a reply is non-empty and then
    returns this reply.

    \return Packet contaning a non-empty reply. */
  CCRTPPacket *waitForPacket();

  /*! \brief Whether or not the copter is answering sent packets.

    Returns whether the copter is actually answering sent packets with
    a set ACK flag. If this is not the case, it is either switched off
    or out of range.

    \return Returns true if the copter is returning the ACK flag properly, false otherwise. */
  bool ackReceived();
  /*! \brief Whether or not the USB connection is still operational.

    Checks if the USB read/write calls yielded any errors.

    \return Returns true if the connection is working properly and
    false otherwise. */
  bool usbOK();

  /*! \brief Extracting all logging related packets

    Returns a list of all collected logging related (i.e. originating
    from port 5) packets. This is called by the CCrazyflie class
    automatically when performing cycle().

    \return List of CCRTPPacket instances collected from port 5
    (logging). */
  std::list<CCRTPPacket*> popLoggingPackets();
};


#endif /* __C_CRAZY_RADIO_H__ */
