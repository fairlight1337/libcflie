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


#ifndef __C_CRTP_PACKET_H__
#define __C_CRTP_PACKET_H__


// System
#include <cstring>


/*! \brief Class to hold and process communication-related data for
  the CRTProtocol */
class CCRTPPacket {
 private:
  // Variables
  /*! \brief Internal storage pointer for payload data inside the
    packet

    This data is freed when either new data is set or the class
    instance is destroyed.*/
  char *m_cData;
  /*! \brief The length of the data pointed to by m_cData */
  int m_nDataLength;
  /*! \brief The copter port the packet will be delivered to */
  int m_nPort;
  /*! \brief The copter channel the packet will be delivered to */
  int m_nChannel;
  bool m_bIsPingPacket;

  // Functions
  /*! \brief Sets all internal variables to their default values.

    The function clearData() should be called before this if it is
    used outside of the constructor. */
  void basicSetup();
  /*! \brief Deletes the internally stored data and resets the data
    length and the pointer to zero */
  void clearData();

 public:
  /*! \brief Constructor for the CCRTPPacket communication packet
    class

    Initializes the communication packet and sets the given
    channel. The packet starts out without payload data.

    \param nChannel The channel the payload in this packet is
    designated for. */
  CCRTPPacket(int nChannel);
  /*! \brief Convenience constructor for the CCRTPPacket communication
    packet class

    Initializes the communication packet and sets the given
    channel. The given data is set as the internal payload data.

    \param cData The data pointer to read the new payload data from
    \param nDataLength The length (in bytes) of data to read from
    cData
    \param nChannel The channel the payload in this packet is
    designated for. */
  CCRTPPacket(char *cData, int nDataLength, int nChannel);
  CCRTPPacket(char cData, int nPort);
  /*! \brief Destructor for the packet class

    De-initializes the packet and deletes all available payload data
    stored. */
  ~CCRTPPacket();

  /*! \brief Copies the given data of the specified length to the
    internal storage.

    \param cData Pointer pointing to the data that should be used as
    payload
    \param nDataLength Length (in bytes) of the data that should be
    read from cData for storage */
  void setData(char *cData, int nDataLength);
  /*! \brief Gives out the pointer to the internally stored data

    Don't manipulate the data pointed to by this pointer. Usually, you
    won't have to call this function at all as it is used by the more
    interface-designated functions.

    \return Returns a direct pointer to the internally stored data */
  char *data();
  /*! \brief Returns the length of the currently stored data (in
      bytes)

    \return Returns the number of bytes stored as payload data */
  int dataLength();

  /*! \brief Prepares a sendable block of data based on the
      CCRTPPacket details

    A block of data is prepared that contains the packet header
    (channel, port), the payload data and a finishing byte
    (0x27). This block is newly allocated and must be delete[]'d after
    usage.

    \return Pointer to a new char[] containing a sendable block of
    payload data */
  char *sendableData();
  /*! \brief Returns the length of a sendable data block

    \return Length of the sendable data block returned by
    sendableData() (in bytes) */
  int sendableDataLength();

  /*! \brief Set the copter port to send the payload data to

    The port identifies the purpose of the packet on the copter. This
    function sets the port that is later used in sendableData().

    \param nPort Port number to set */
  void setPort(int nPort);
  /*! \brief Returns the currently set port number */
  int port();

  /*! \brief Set the copter channel to send the payload data to

    The channel identifies the purpose of the packet on the
    copter. This function sets the channel that is later used in
    sendableData().

    \param nChannel Channel number to set */
  void setChannel(int nChannel);
  /*! \brief Returns the currently set channel number */
  int channel();

  void setIsPingPacket(bool bIsPingPacket);
  bool isPingPacket();
};


#endif /* __C_CRTP_PACKET_H__ */
