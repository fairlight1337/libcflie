//
// Copyright (c) 2014 Alexander Holler <holler@ahsoftware.de>
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

/// \author Alexander Holler

#ifndef CCRTPPACKET_H
#define CCRTPPACKET_H

#include <string>

class CCRTPPacket : std::string {
	public:
		enum Channel {
			ChannelTOC,
			ChannelRead,
			ChannelWrite,
		};
		enum Port {
			PortConsole = 0x00,
			PortParam = 0x20,
			PortCommander = 0x30,
			PortLogging = 0x50,
			PortDebugDriver = 0xE0,
			PortLinkCtrl = 0xF0,
		};

		CCRTPPacket()
			: std::string(1, 0x0c)
		{}
		CCRTPPacket(const std::string& s)
			: std::string(s)
		{}
		CCRTPPacket(std::string&& s)
			: std::string(std::move(s))
		{}

		CCRTPPacket(const char *cData, int nDataLength, Port nPort)
			: std::string(std::string(1, nPort | 0x0c) + std::string(cData, nDataLength))
		{}
		CCRTPPacket(char cData, Port nPort)
			: std::string(std::string(1, nPort | 0x0c) + std::string(1, cData))
		{}

		void setData(const std::string& cData) {
			*this = std::string(1, at(0)) + cData;
		}
		void setData(std::string&& cData) {
			*this = std::string(1, at(0)) + std::move(cData);
		}
		const char *payload(void) const {
			return data()+1;
		}
		size_t payloadLength(void) const {
			return size()-1;
		}

		const char *sendableData(void) const {
			return data();
		}
		size_t sendableDataLength() const {
			return size();
		}

		void setPort(Port nPort) {
			(*this)[0] = (at(0) & ~0xf0) | nPort;
		}
		Port port(void) const {
			return Port(at(0) & 0xf0);
		}

		void setChannel(Channel nChannel) {
			(*this)[0] = (at(0) & ~0x03) | nChannel;
		}
		Channel channel(void) const {
			return Channel(at(0) & 0x03);
		}
};

class CCRTPPacketPing : public CCRTPPacket
{
	public:
		CCRTPPacketPing()
			: CCRTPPacket()
		{
			setPort(PortLinkCtrl);
			setChannel(Channel(0x03));
		}
};

#endif // CCRTPPACKET_H
