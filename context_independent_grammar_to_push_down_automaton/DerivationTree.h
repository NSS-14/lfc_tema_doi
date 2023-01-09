#pragma once
#include <string>
#include <vector>
#include <iostream>

class DerivationTree
{
public:
	class Node {
	public:
		Node(const std::string& symbols, Node* parent = nullptr);

	public:
		const std::string& GetSymbols() const;
		Node* GetParent() const;
		const std::vector<Node*>& GetChildrens() const;
		bool HasChildrens() const;

	public:
		void SetSymbols(const std::string& symbols);
		void SetParent(Node* parent);
		void SetChildrens(const std::vector<Node*>& childrens);

	public:
		void AddChildren(Node* children);
		void ClearChildrens();

	private:
		std::string m_symbols;
		Node* m_parent;
		std::vector<Node*>m_childrens;
	};
public:
	DerivationTree() = default;
	DerivationTree(const std::string& startSymbol);
	DerivationTree(const DerivationTree& derivationTree);
	~DerivationTree();

public:
	bool operator ==(const DerivationTree& derivationTree) const;
	DerivationTree& operator =(const DerivationTree& derivationTree);
	friend std::ostream& operator <<(std::ostream& out, const DerivationTree& derivationTree);

private:
	bool VerifyIfVectorContainsNode(const std::vector<Node*> vector, Node* node) const;

public:
	unsigned int GetLongestPath() const;
	Node* GetRoot() const;
	std::vector<Node*> GetCross() const;
	std::vector<Node*> GetLeaves() const;
	std::string GetResult() const;
	bool Empty() const;

public:
	void Clear();

private:
	Node* m_root = nullptr;
};

