#pragma once

#include <string>
#include <vector>
#include <iostream>

class Tag
{
protected:
	std::string name;
	std::string text;
	std::vector<Tag> children;
	std::vector<std::pair<std::string, std::string>> attributes;

public:
	friend std::ostream& operator<<(std::ostream& os, const Tag& tag)
	{
		os << "<" << tag.name;
		for (auto& item : tag.attributes) {
			os << " " << item.first << " : " << item.second;
		}
		os << ">" << std::endl;
		os << tag.text << std::endl;
		for (auto& item : tag.children) {
			os << item;
		}
		os << "</" << tag.name << ">" << std::endl;
		return os;
	}

	Tag(const std::string& name, const std::string& text) 
		: name{ name }, text{ text } {}
	Tag(const std::string& name, const std::vector<Tag>& children)
		: name{ name }, children{ children } {}
};