#pragma once

#include "Tag.h"

class P : public Tag
{
public:
	explicit P(const std::string& text)
		: Tag{ "p", text } {}

	P(std::initializer_list<Tag> children)
		: Tag{ "p", children } {}
};

class IMG : public Tag
{
public:
	explicit IMG(const std::string& url)
		: Tag{ "img", "" }
	{
		attributes.emplace_back(std::make_pair( "src", url ));
	}
};