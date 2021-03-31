#pragma once

#include <string>
#include <ostream>

class Token
{
public:
	enum Type { interger, plus, minus, lparen, rparen } type;
	std::string text;

	explicit Token(Type type, const std::string& text)
		: type{ type }, text{ text } {}

	friend std::ostream& operator<<(std::ostream& os, const Token& obj)
	{
		return os << "'" << obj.text << "'";
	}
};