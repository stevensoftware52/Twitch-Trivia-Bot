// Steven Rogers

#ifndef _TRIVIA_H
#define _TRIVIA_H

#include "stdafx.h"

class TwitchIRC;

class Trivia
{
	public:
		Trivia(TwitchIRC* pOwner);
		virtual ~Trivia() {}

	private:
		void LoadScore();

		TwitchIRC* m_pOwner;
};

#endif