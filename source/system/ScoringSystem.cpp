#include "pch.h"

#include "ScoringSystem.h"

int ScoringSystem::getLevelCount() const
{
	return total_level_cleared;
}

bool ScoringSystem::firstLevel() const
{
	return total_level_cleared == 0;
}

void ScoringSystem::incrementLevelCleared()
{
	total_level_cleared++;
}

void ScoringSystem::reset()
{
	//pass data to scorebaord
	//parse_date_to_file();
	total_level_cleared = 0;
}