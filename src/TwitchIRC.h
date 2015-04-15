// Steven Rogers

#ifndef _TWITCHIRC_H
#define _TWITCHIRC_H

#include "stdafx.h"

class TwitchIRC
{
	public:
	    TwitchIRC(std::string nick, std::string usr, std::string addr, uint32 port, std::string channel, std::string password = "");
	    ~TwitchIRC();

		void endSocket()
		{
			closesocket(m_sSocket);
			m_sSocket = NULL;
		}

		bool activeSocket() { return m_sSocket != NULL; }

	private:
		bool ReceiveIRCMessage(std::string& message);

	    SOCKET m_sSocket;
		WSADATA m_wsa;
		
		std::string m_channelName;
};

#endif