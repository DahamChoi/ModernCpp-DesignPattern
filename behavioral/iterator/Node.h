#pragma once

template<typename T>
class BinaryTree;

template<typename T> class Node
{
public:
	T value;
	Node<T>* left = nullptr;
	Node<T>* right = nullptr;
	Node<T>* parent = nullptr;
	BinaryTree<T>* tree = nullptr;

	explicit Node(const T& value)
		: value(value) {}

	Node(const T& value, Node<T>* const left, Node<T>* const right)
		: value(value), left(left), right(right)
	{
		this->left->tree = this->right->tree = tree;
		this->left->parent = this->right->parent = this;
	}

	void set_tree(BinaryTree<T>* t)
	{
		tree = t;
		if (left) left->set_tree(t);
		if (right) right->set_tree(t);
	}
};