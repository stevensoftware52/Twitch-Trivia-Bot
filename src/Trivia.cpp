// Steven Rogers

#include "stdafx.h"

Trivia::Trivia(TwitchIRC* pOwner) :
	m_pOwner(pOwner),
	m_iQuestionExpireTimer(0),
	m_iNextQuestionTimer(0),
	m_uiCurQuestion(0),
	m_ctLastUpdate(clock())
{
	// Need a TwitchIRC for this class to work
	ASSERT(m_pOwner);

	LoadScore();
	LoadTrivia();
}

void Trivia::Update()
{
	const clock_t timeDiff = clock() - m_ctLastUpdate;

	// We need at least 1 ms change since we aren't dealing with nanoseconds here!
	if (!timeDiff)
		return;

	m_ctLastUpdate = clock();

	// Process delayed messages before anything below. 
	// What we don't want is to queue something then subtract time right after.
	auto itr = m_vDelayedMsgs.begin();

	while (itr != m_vDelayedMsgs.end())
	{
		(*itr).timer -= int32(timeDiff);

		if ((*itr).timer <= 0)
		{
			m_pOwner->SendChatMsg((*itr).msg);
			itr = m_vDelayedMsgs.erase(itr);
		}
		else
			++itr;
	}

	// The timer for when a question expires (no one answered in time)
	if (m_iQuestionExpireTimer > 0)
	{
		m_iQuestionExpireTimer -= int32(timeDiff);

		if (m_iQuestionExpireTimer <= 0)
		{
			const TriviaQA& triviaRef = m_vQuestions[m_uiCurQuestion];
			
			std::string answerString;
			for (size_t i = 0; i < triviaRef.answers.size(); ++i)
				answerString.append(triviaRef.answers[i]);

			m_vDelayedMsgs.clear();
			
			// Some randomization so that twitch doesn't see us sending messages at exact intervals
			m_pOwner->SendChatMsg("Time has expired!");
			queueMessage("The answer(s): " + answerString, randomChoice(1000, 2000));
			queueNextQuestion();
		}
	}

	// Is it time for another question?
	if (m_iNextQuestionTimer > 0)
	{
		m_iNextQuestionTimer -= int32(timeDiff);

		if (m_iNextQuestionTimer <= 0)
			BeginNewQA();
	}
}

void Trivia::BeginNewQA()
{
	m_vDelayedMsgs.clear();

	// Move on to next question
	++m_uiCurQuestion;

	if (m_uiCurQuestion >= m_vQuestions.size())
		m_uiCurQuestion = 0;

	// You get 60 seconds to answer, but it takes 10 seconds for the preamble to occur.
	m_iQuestionExpireTimer = 70000;

	// Send the header for a new trivia
	m_pOwner->SendChatMsg(getTopScorePrint());
	queueMessage("Trivia #" + std::to_string(m_uiCurQuestion) + " is worth " + std::to_string(m_vQuestions[m_uiCurQuestion].points) + " points!", randomChoice(2000, 3000));
	queueMessage("-- TRIVIA #" + std::to_string(m_uiCurQuestion) + " --", randomChoice(6000, 7000));
	queueMessage(m_vQuestions[m_uiCurQuestion].question, randomChoice(10000, 11000));

	// These messages would let people know how much time is left.
	queueMessage("Time will expire in 30 seconds!", 10000 + randomChoice(28000, 30000));
	queueMessage("Hurry, time will expire in 15 seconds!", 10000 + randomChoice(43000, 44000));
	queueMessage("Only five seconds remain!", 10000 + randomChoice(52000, 54000));
}

void Trivia::ProcessAnswer(const std::string name, const std::string answer)
{
	if (!isQuestionInProgress())
		return;

	if (isAnswerCorrect(m_uiCurQuestion, answer))
	{
		m_vDelayedMsgs.clear();

		uint8 points = m_vQuestions[m_uiCurQuestion].points;

		m_uoScore[name] += points;

		std::string answerString;

		for (size_t i = 0; i < m_vQuestions[m_uiCurQuestion].answers.size(); ++i)
			answerString.append(m_vQuestions[m_uiCurQuestion].answers[i]);
		
		queueMessage("He has done it! User " + name + " gains " + std::to_string(points) + " points for a total of " + std::to_string(m_uoScore[name]) + "!", 1000);
		queueMessage("The answer(s): " + answerString, randomChoice(3000, 4000));
		SaveScore();
		queueNextQuestion(false);
	}
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

	// Start off at a random index, because it's more fun that way!
	m_uiCurQuestion = Util::randomNumber(0, m_vQuestions.size() - 1);
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