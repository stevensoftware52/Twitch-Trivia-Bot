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

		void Update();
		void ProcessAnswer(const std::string name, const std::string answer);
		
		void queueNextQuestion(const bool bCheckIfOneExists = true)
		{
			if (bCheckIfOneExists && isQuestionInProgress())
				return;

			m_iNextQuestionTimer = randomNumber(8000, 10000);
			m_iQuestionExpireTimer = 0;
		}

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

		bool isQuestionInProgress() { return m_iNextQuestionTimer <= 0 && m_iQuestionExpireTimer > 0; }

		// Not very sophisticated but it's good enough to resemble how something like WoW Trivia Bot worked.
		bool isAnswerCorrect(size_t qaIndex, std::string answer) const
		{
			if (qaIndex >= m_vQuestions.size())
			{
				printf("Trivia::isAnswerCorrect - Invalid index provided, %d\n", qaIndex);
				return false;
			}

			std::transform(answer.begin(), answer.end(), answer.begin(), ::tolower);
			
			bool bGood = false;

			for (size_t i = 0; i < m_vQuestions[qaIndex].answers.size(); ++i)
			{
				// Really short answer, only accept exact match
				if (m_vQuestions[qaIndex].answers[i].size() <= 3)
				{
					if (m_vQuestions[qaIndex].answers[i] == answer)
						bGood = true;
				}

				// Might be lazy, but just see if the answer contains their answer as long as they have what we think is a full word
				else if (answer.size() > 3)
				{
					if (m_vQuestions[qaIndex].answers[i].find(answer) != std::string::npos || answer.find(m_vQuestions[qaIndex].answers[i]) != std::string::npos)
						bGood = true;
				}
			}

			return bGood;
		}

	private:
		void LoadScore();
		void SaveScore();
		void LoadTrivia();
		void BeginNewQA();

		void queueMessage(const std::string msg, int32 timer)
		{
			DelayedMsg dm;
			dm.msg = msg;
			dm.timer = timer;
			m_vDelayedMsgs.push_back(dm);
		}

		std::string getTopScorePrint() const
		{
			uint32 largestScore = 0;
			std::string highScoreName;

			for (auto itr = m_uoScore.begin(); itr != m_uoScore.end(); ++itr)
			{
				if (itr->second > largestScore)
				{
					largestScore = itr->second;
					highScoreName = itr->first;
				}
			}

			if (highScoreName.size())
				return "User '" + highScoreName + "' has top score with " + std::to_string(largestScore) + " points!";

			return "Currently, no users have scored any points.";
		}

		int32 m_iQuestionExpireTimer;
		int32 m_iNextQuestionTimer;

		uint32 m_uiCurQuestion;

		clock_t m_ctLastUpdate;

		TwitchIRC* m_pOwner;

		struct DelayedMsg
		{
			int32 timer;
			std::string msg;
		};

		std::vector<TriviaQA> m_vQuestions;
		std::vector<DelayedMsg> m_vDelayedMsgs;

		// name][score
		std::unordered_map<std::string, uint32> m_uoScore;
};

#endif