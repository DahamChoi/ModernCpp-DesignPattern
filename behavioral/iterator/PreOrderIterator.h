#pragma once

template<typename T>
class Node;

template <typename U>
class PreOrderIterator
{
public:
	Node<U>* current;

	explicit PreOrderIterator(Node<U>* current)
		: current(current) {}

	bool operator!=(const PreOrderIterator<U>& other)
	{
		return current != other.current;
	}

	class Node<U>& operator*() { return *current; }

	PreOrderIterator<U>& operator++()
	{
		if (current->right)
		{
			current = current->right;
			while (current->left) {
				current = current->left;
			}
		}
		else
		{
			Node<U>* p = current->parent;
			while (p && current == p->right)
			{
				current = p;
				p = p->parent;
			}
			current = p;
		}

		return *this;
	}
};