#include <iostream>

#include "DummyDatabase.h"
#include "ConfigurableRecordFinder.h"

int main()
{
    DummyDatabase db{};
    ConfigurableRecordFinder rf{ db };

    std::cout <<
        rf.total_population(std::vector<std::string>{"seoul", "tokyo"}) << std::endl;
}