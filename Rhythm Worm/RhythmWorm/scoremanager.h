#pragma once

enum RankThreshold
{
	F_RANK = 0,
	E_RANK = 5000,
	D_RANK = 10000,
	C_RANK = 20000,
	B_RANK = 30000,
	A_RANK = 50000,
	S_RANK = 70000
};

enum ScoreValue
{
	BAD_SCORE = -500,
	GOOD_SCORE = 100,
	BEST_SCORE = 500
};

class ScoreManager
{
public:
	ScoreManager();
	~ScoreManager();

	int GetFinalScore() const;
	int GetBestHits() const;
	int GetGoodHits() const;
	int GetBadHits() const;
	void AddScore(ScoreValue _score);
	void Reset();

private:
	int bestHits;		//The amount of perfect beats hit
	int goodHits;		//The amount of slightly off-beats hit
	int badHits;		//The amount of hits missed (hit the wrong beat or missed a hit)

	int finalScore;		//The final calculated score
};