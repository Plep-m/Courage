#include <tuple>
#include <cmath>

namespace Courage::Logic
{

	class Experience
	{
	public:
		static std::tuple<int, float> calculateLevelAndBar(int totalExperience);
		static int calculateLevel(int totalExp);
		static float calculateExperienceBar(int totalExp, int level);
		static int calculateExpForLevel(int level);
	};

}