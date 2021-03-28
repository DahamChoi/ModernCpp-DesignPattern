#pragma once

#include <vector>
#include <string>

#include "SingletonDatabase.h"

class SingletonRecordFinder
{
public:
	// �� �Լ������� SingletonDatabase�� �����ϰ� ���������ν� ������ �߻��Ѵ�!
	// ���� ��� SingletonRecordFinder�� ���� ���� �׽�Ʈ�� �Ѵٰ� �����غ���
	// �̷��� ���迡���� ���� ������Ʈ�� ������ �����͸� ������ ����ؾ� �Ѵ�.
	int total_population(std::vector<std::string> names)
	{
		int result = 0;
		for (auto& name : names)
			result += SingletonDatabase::get().get_population(name);
		return result;
	}
};