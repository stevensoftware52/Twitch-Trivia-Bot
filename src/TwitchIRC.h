// Steven Rogers

#ifndef _TWITCHIRC_H
#define _TWITCHIRC_H

#include "stdafx.h"

class TwitchIRC
{
	public:
	    TwitchIRC(const std::string nick, const std::string usr, const std::string addr, const uint32 port, const std::string channel, const std::string password = "");
	    ~TwitchIRC();

		void Update();

		void endSocket()
		{
			closesocket(m_sSocket);
			m_sSocket = NULL;
		}

		// Returns false if failed.
		bool SendChatMsg(std::string chatMsg);

		bool activeSocket() { return m_sSocket != NULL; }

	private:

		// IRC messages come with formatting; this strips it apart a PRIVMSG from IRC to tell us who sent it and what he sent.
		void StripPRIVMSG(const std::string ircMsg, std::string& username, std::string& msg);
		void SendPong(const std::string incomingMsg);
		
		// Returns false if failed.
		bool ReceiveIRCMessage(std::string& message);

	    SOCKET m_sSocket;
		WSADATA m_wsa;
		
		std::string m_channelName;
};

#endif