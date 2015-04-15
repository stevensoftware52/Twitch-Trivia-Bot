// Steven Rogers

#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{
	TwitchIRC irc("gummy52", "gummy52", "irc.twitch.tv", 6667, "#gummy52", "oauth:k5lda5anbr86bpfxhzofw3sjxlomxy");

	while (irc.activeSocket())
	{
		irc.Update();
	}

	printf("The program has ended.\n");
	system("pause");
	return 0;
}

