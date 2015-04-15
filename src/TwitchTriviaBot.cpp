// Steven Rogers

#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{
	std::vector<std::string> config = Util::readLinesFromFile("login.txt");

	if (config.size() >= 5)
	{
		TwitchIRC irc(config[0], config[0], config[1], (uint32)atoi(config[2].c_str()), config[3], config[4]);

		while (irc.activeSocket())
		{
			irc.Update();
		}
	}
	else
		printf("There was an issue reading login.txt.\n");

	printf("The program has ended.\n");
	system("pause");
	return 0;
}

