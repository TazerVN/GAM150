#pragma once
#include <vector>
#include <utility>

template<typename T, typename W>
class Node
{
	private:
	T first;
	W second;
	Node<T, W>* left;
	Node<T, W>* right;

	public:
	Node(T z, W ID) : first{ z }, second{ ID }, left{ nullptr }, right{ nullptr } {}
	T getFirst() { return first; }
	W getSecond() { return second; }
	Node<T, W>*& getLeft() { return left; }
	Node<T, W>*& getRight() { return right; }
};


template<typename T, typename W>
class BinaryTree
{
	private:
	Node<T, W>* root;
	Node<T, W>* insert(Node<T, W>* node, T first, W second)
	{
		if (!node) return new Node<T, W>{ first, second };
		if (first <= node->getFirst())
			node->getLeft() = insert(node->getLeft(), first, second);
		else if (first > node->getFirst())
			node->getRight() = insert(node->getRight(), first, second);
		return node;
	}
	void destroy(Node<T, W>* node)
	{
		if (!node) return;
		destroy(node->getLeft());
		destroy(node->getRight());
		delete node;
	}
	void inOrder(Node<T, W>* node, std::vector<std::pair<T, W>>& result)
	{
		if (!node) return;
		inOrder(node->getLeft(), result);
		result.push_back(std::pair<T, W>{node->getFirst(), node->getSecond()});
		inOrder(node->getRight(), result);
	}
	Node<T, W>*& getSuccessor(Node<T, W>*& curr)
	{
		curr = curr->getRight();
		while (curr != nullptr && curr->getLeft() != nullptr)
			curr = curr->getLeft();
		return curr;
	}

	Node<T, W>*& delNode(Node<T, W>*& root, T z, W id)
	{
		if (root == nullptr)
			return root;

		if (root->getSecond() != id && root->getFirst() > z )
			root->getLeft() = delNode(root->getLeft(), z, id);
		else if (root->getSecond() != id && root->getFirst() < z)
			root->getRight() = delNode(root->getRight(), z, id);
		else
		{
	  // Node with 0 or 1 child
			if (root->getLeft() == nullptr)
			{
				Node<T, W>* temp = root->getRight();
				delete root;
				return temp;
			}
			if (root->getRight() == nullptr)
			{
				Node<T, W>* temp = root->getLeft();
				delete root;
				return temp;
			}

			// Node with 2 children
			Node<T,W>* succ = getSuccessor(root);
			root->getFirst() = succ->getFirst();
			root->getSecond() = succ->getSecond();
			root->getRight() = delNode(root->getRight(), succ->getFirst(), succ->getSecond());
		}
		return root;
	}


	public:
	BinaryTree() : root{ nullptr } {}
	~BinaryTree() { destroy(root); }
	void insert(T z, W ID)
	{
		root = this->insert(root, z, ID);
	}
	void inOrder(std::vector<std::pair<T, W>>& result)
	{
		this->inOrder(root, result);
	}
	void destroy(){
		this->destroy(root);
		this->root = nullptr;
	}

};

