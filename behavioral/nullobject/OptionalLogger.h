#pragma once

#include "Logger.h"

#include <memory>

class OptionalLogger : public Logger
{
private:
	std::shared_ptr<Logger> impl;

public:
	static std::shared_ptr<Logger> no_logging;

	OptionalLogger(const std::shared_ptr<Logger> logger) : impl{ logger } {}
	
	virtual void info(const std::string& s) override {
		if (impl) impl->info(s);
	}
	
	virtual void warn(const std::string& s) override {
		if (impl) impl->warn(s);
	}
};

std::shared_ptr<Logger> OptionalLogger::no_logging{};