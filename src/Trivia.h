// Steven Rogers

#ifndef _TRIVIA_H
#define _TRIVIA_H

#include "stdafx.h"

class TwitchIRC;

// The letter that separates one phrase from another
//		Example,	"One	Two	Three	Four	Five	etc..."	
#define TRIVIA_WORDS_SEPERATOR '\t'

struct TriviaQA
{
	TriviaQA() :
		points(1)
	{}

	std::string question;
	std::vector<std::string> answers;
	uint8 points;
};

class Trivia
{
	public:
		Trivia(TwitchIRC* pOwner);
		virtual ~Trivia() {}

		static void getWordsSeperatedBy(const std::string line, const char seperator, std::vector<std::string>& result)
		{
			std::string content;

			for (size_t i = 0; i < line.size(); ++i)
			{
				if (line[i] == seperator)
				{
					result.push_back(content);
					content.clear();
				}
				else
				{
					content.push_back(line[i]);
				}
			}

			if (content.size())
				result.push_back(content);
		}

	private:
		void LoadScore();
		void SaveScore();
		void LoadTrivia();

		TwitchIRC* m_pOwner;

		std::vector<TriviaQA> m_vQuestions;

		// name][score
		std::unordered_map<std::string, uint32> m_uoScore;
};

#endif