// Steven Rogers

#include "stdafx.h"

Trivia::Trivia(TwitchIRC* pOwner) :
	m_pOwner(pOwner)
{
	// Need a TwitchIRC for this class to work
	ASSERT(m_pOwner);

	LoadScore();
	LoadTrivia();
}

void Trivia::LoadTrivia()
{
	m_vQuestions.clear();

	// Question
	// Answer1\tAnswer2\tAnswer3, etc
	// Points awarded

	std::vector<std::string> lines = Util::readLinesFromFile("trivia.txt");

	for (size_t i = 0; i < lines.size(); i += 4)
	{
		if (i + 2 >= lines.size())
		{
			printf("Trivia::LoadTrivia - There was an issue at line %d.\n", i);
			break;
		}

		TriviaQA trivia;
		trivia.question = lines[i];
		getWordsSeperatedBy(lines[i + 1], TRIVIA_WORDS_SEPERATOR, trivia.answers);
		trivia.points = (uint8)atoi(lines[i + 2].c_str());

		if (!trivia.question.size())
			printf("Trivia::LoadTrivia - Invalid question at line %d.\n", i);

		if (!trivia.answers.size())
			printf("Trivia::LoadTrivia - Invalid answers at line %d.\n", i);

		if (!trivia.points)
			printf("Trivia::LoadTrivia - Invalid points at line %d.\n", i);

		m_vQuestions.push_back(trivia);
	}

	printf("Loaded %d trivia questions.\n", m_vQuestions.size());
}

void Trivia::SaveScore()
{
    std::ofstream file("scoreBoard.txt");

	if (file.is_open())
	{
		for (auto itr = m_uoScore.begin(); itr != m_uoScore.end(); ++itr)
			file << itr->first << TRIVIA_WORDS_SEPERATOR << itr->second << "\n";

		file.close();
	}
}

void Trivia::LoadScore()
{
	std::vector<std::string> scores = Util::readLinesFromFile("scoreboard.txt");

	for (size_t i = 0; i < scores.size(); ++i)
	{
		std::vector<std::string> words;
		getWordsSeperatedBy(scores[i], TRIVIA_WORDS_SEPERATOR, words);

		if (words.size() == 2)
			m_uoScore[words[0]] = (uint32)atoi(words[1].c_str());
		else
			printf("Trivia::LoadScore - Issue reading score at line %d, found %d words\n", i, words.size());
	}

	SaveScore();
}