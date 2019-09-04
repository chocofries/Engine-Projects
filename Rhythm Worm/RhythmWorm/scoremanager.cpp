#include "scoremanager.h"

ScoreManager::ScoreManager()
{
	finalScore = 0;
	badHits = 0;
	goodHits = 0;
	bestHits = 0;
}

ScoreManager::~ScoreManager()
{
}

int ScoreManager::GetFinalScore() const
{
	return finalScore;
}

int ScoreManager::GetBestHits() const
{
	return bestHits;
}

int ScoreManager::GetGoodHits() const
{
	return goodHits;
}

int ScoreManager::GetBadHits() const
{
	return badHits;
}

void ScoreManager::AddScore(ScoreValue _score)
{
	switch (_score)
	{
	case BAD_SCORE:
	{
		badHits += 1;
		finalScore += BAD_SCORE;
		break;
	}
	case GOOD_SCORE:
	{
		goodHits += 1;
		finalScore += GOOD_SCORE;
		break;
	}
	case BEST_SCORE:
	{
		bestHits += 1;
		finalScore += BEST_SCORE;
		break;
	}
	}
}

void ScoreManager::Reset()
{
	bestHits = 0;
	goodHits = 0;
	badHits = 0;
	finalScore = 0;
}
