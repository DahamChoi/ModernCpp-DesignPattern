#pragma once

#include <string>
#include <vector>
#include <ostream>

class FormattedText
{
public:
	class TextRange
	{
	public:
		int start, end;
		bool capitalize;
		// bool, itealic 등 다른 옵션들도 여기에 둘 수 있다.

		bool covers(int position) const
		{
			return (position >= start && position <= end);
		}
	};

	FormattedText(const std::string& plain_text)
		: plain_text{ plain_text } {}

	TextRange& get_range(int start, int end)
	{
		formattig.emplace_back(TextRange{ start, end });
		return *formattig.rbegin();
	}

	friend std::ostream& operator<<(std::ostream& os, const FormattedText& obj)
	{
		std::string s;
		for (size_t i = 0; i < obj.plain_text.length(); i++)
		{
			auto c = obj.plain_text[i];
			for (const auto& rng : obj.formattig)
			{
				if (rng.covers(i) && rng.capitalize)
					c = toupper(c);
				s += c;
			}
		}

		return os << s;
	}

private:
	std::string plain_text;
	std::vector<TextRange> formattig;
};