#pragma once

template<typename T>
class Node;

template<typename T>
class PreOrderIterator;

template<typename T>
class pre_order_traversal;

template <typename T> class BinaryTree
{
public:
	Node<T>* root = nullptr;

	pre_order_traversal<T> pre_order;

	explicit BinaryTree(Node<T>* root)
		: root{ root }, pre_order{ *this }
	{
		root->set_tree(this);
	}

	typedef PreOrderIterator<T> iterator;

	iterator begin()
	{
		Node<T>* n = root;

		if (n)
			while (n->left)
				n = n->left;

		return iterator{ n };
	}

	iterator end()
	{
		return iterator(nullptr);
	}
};