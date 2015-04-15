// Steven Rogers

#include "stdafx.h"

TwitchIRC::TwitchIRC(std::string nick, std::string usr, std::string addr, uint32 port, std::string channel, std::string password) :
	m_channelName(channel),
	m_sSocket(NULL)
{
	if (int32 err = WSAStartup(MAKEWORD(2, 0), &m_wsa))
	{
		printf("WSAStartup failed with error: %d\n", err);
	}
	else
	{
		printf("TwitchIRC: Constructed.\n");

		m_sSocket = Util::createSocketAndConnect(addr.c_str(), port, false, 10000);

		if (m_sSocket == INVALID_SOCKET)
		{
			printf("Failed to connect to %s.\n", addr.c_str());
			return;
		}
		
		std::string passSend = std::string("PASS " + password + "\r\n");
		std::string nickSend = std::string("NICK " + nick + "\r\n");
		std::string userSend = std::string("USER " + usr + " 0 * :" + usr + "\r\n");

		// The PASS command is used to set a 'connection password'.
		// The optional password can and MUST be set before any attempt to register the connection is made. 
		if (password.size())
			sendBytes(m_sSocket, (char*)passSend.c_str(), passSend.size());

        sendBytes(m_sSocket, (char*)nickSend.c_str(), nickSend.size());
		sendBytes(m_sSocket, (char*)userSend.c_str(), userSend.size());
		
		// Wait for a response.
		std::string response;
		ReceiveIRCMessage(response);
	}
} 

TwitchIRC::~TwitchIRC()
{
	if (m_sSocket)
		closesocket(m_sSocket);

	WSACleanup();
}

bool TwitchIRC::ReceiveIRCMessage(std::string& message)
{
	// IRC doesn't tell us how big the incoming packet is, we just grab data until we see a "\r\n"
	while (activeSocket())
	{
		char letter;
		int32 result = recv(m_sSocket, &letter, 1, 0);
		message.push_back(letter);

		if (result == SOCKET_ERROR)
		{
			endSocket();
			return false;
		}

		if (message.size() > 1 && message[message.size() - 2] == '\r' &&message[message.size() - 1] == '\n')
			return true;

		// Would never get this big. If this has happened, shut it down.
		if (message.size() > MAX16BIT)
			endSocket();
	}

	return false;
}