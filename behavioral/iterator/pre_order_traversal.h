#pragma once

template<typename T>
class BinaryTree;

template<typename T>
class PreOrderIterator;

template<typename T>
class pre_order_traversal
{
private:
	BinaryTree<T>& tree;

public:
	pre_order_traversal(BinaryTree<T>& tree)
		: tree(tree) {}

	typedef PreOrderIterator<T> iterator;
	
	iterator begin() { return tree.begin(); }
	iterator end() { return tree.end(); }
};