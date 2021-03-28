#pragma once

#include <vector>
#include <string>

#include "SingletonDatabase.h"

class SingletonRecordFinder
{
public:
	// 이 함수에서는 SingletonDatabase에 밀접하게 의존함으로써 문제가 발생한다!
	// 예를 들어 SingletonRecordFinder에 대한 단위 테스트를 한다고 가정해보자
	// 이러한 설계에서는 더미 컴포넌트를 일일히 데이터를 주입해 사용해야 한다.
	int total_population(std::vector<std::string> names)
	{
		int result = 0;
		for (auto& name : names)
			result += SingletonDatabase::get().get_population(name);
		return result;
	}
};