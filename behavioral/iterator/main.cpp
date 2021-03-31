#include <iostream>
#include <string>

#include "BinaryTree.h"
#include "Node.h"
#include "PreOrderIterator.h"
#include "pre_order_traversal.h"

int main()
{
	BinaryTree<std::string> family{
		new Node<std::string>{"me",
			new Node<std::string>{"mother",
				new Node<std::string>{"mother's mother"},
				new Node<std::string>{"mother's father"}
			},
			new Node<std::string>{"father"}
		}
	};

	for (const auto& it : family.pre_order)
	{
		std::cout << it.value << std::endl;
	}
}