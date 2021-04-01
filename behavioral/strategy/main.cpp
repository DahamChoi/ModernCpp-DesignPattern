#include "TextStrategy.h"
#include "HtmlListStrategy.h"
#include "MarkdownListStrategy.h"

#include <iostream>

int main()
{
	TextProcessor<MarkdownListStrategy> tpm;
	tpm.append_list({ "foo", "bar", "baz" });
	std::cout << tpm.str() << std::endl;

	TextProcessor<HtmlListStrategy> tph;
	tph.append_list({ "foo", "bar", "baz" });
	std::cout << tph.str() << std::endl;
}