#pragma once
#include <vector>

struct Score
{
	int total_level_cleared = 0;
};

class ScoringSystem 
{
private:
	std::vector<Score> highscores;
	int total_level_cleared = 0;
public:
	int getLevelCount() const;
	bool firstLevel() const;
	void incrementLevelCleared();
	void reset();
};