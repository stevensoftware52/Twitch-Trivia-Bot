// Steven Rogers

#include "stdafx.h"

Trivia::Trivia(TwitchIRC* pOwner) :
	m_pOwner(pOwner)
{
	// Need a TwitchIRC for this class to work
	ASSERT(m_pOwner);
}