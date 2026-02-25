#pragma once
#include <vector>
#include <utility>

template<typename T, typename W>
class Node
{
	private:
	T first;
	W second;
	Node<T,W>* left;
	Node<T,W>* right;

	public:
	Node(T z, W ID) : first{ z }, second{ ID }, left{ nullptr }, right{nullptr} {}
	T getFirst() {return first;}
	W getSecond() { return second;}
	Node<T,W>*& getLeft(){return left;}
	Node<T,W>*& getRight(){return right;}
};


template<typename T, typename W>
class BinaryTree
{
	private:
	Node<T,W>* root;
	Node<T,W>* insert(Node<T,W>* node, T z, W ID)
	{
		if (!node) return new Node<T, W>{ z, ID };
		if (z <= node->getFirst())
			node->getLeft() = insert(node->getLeft(), z, ID);
		else if (z > node->getFirst())
			node->getRight() = insert(node->getRight(), z, ID);
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


	public:
	BinaryTree() : root{nullptr}{}
	~BinaryTree(){destroy(root);}
	void insert(T z, W ID){
		root = this->insert(root, z, ID);
	}
	void inOrder(std::vector<std::pair<T, W>>& result)
	{
		this->inOrder(root, result);
	}

};

