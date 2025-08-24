#include "Courage/Logic/Experience.hpp"

namespace Courage::Logic
{
	std::tuple<int, float> Experience::calculateLevelAndBar(int totalExperience)
	{
		int level = calculateLevel(totalExperience);
		float experienceBar = calculateExperienceBar(totalExperience, level);
		return {level, experienceBar};
	}

	int Experience::calculateLevel(int totalExp)
	{
		if (totalExp < 0)
			return 0;

		if (totalExp < 352)
			return static_cast<int>(std::sqrt(totalExp + 9) - 3);
		else if (totalExp < 1507)
			return static_cast<int>((81.0f + std::sqrt(6561.0f + 72.0f * (totalExp - 352))) / 18.0f);
		else
			return static_cast<int>((325.0f + std::sqrt(105625.0f + 72.0f * (totalExp - 1507))) / 18.0f);
	}

	float Experience::calculateExperienceBar(int totalExp, int level)
	{
		if (level <= 0)
			return 0.0f;

		int expCurrent = calculateExpForLevel(level);
		int expNext = calculateExpForLevel(level + 1);
		int expInLevel = totalExp - expCurrent;
		int expNeeded = expNext - expCurrent;

		return static_cast<float>(expInLevel) / static_cast<float>(expNeeded);
	}

	int Experience::calculateExpForLevel(int level)
	{
		if (level <= 0)
			return 0;
		if (level <= 16)
			return level * level + 6 * level;
		if (level <= 31)
			return static_cast<int>(2.5f * level * level - 40.5f * level + 360.0f);
		return static_cast<int>(4.5f * level * level - 162.5f * level + 2220.0f);
	}
}