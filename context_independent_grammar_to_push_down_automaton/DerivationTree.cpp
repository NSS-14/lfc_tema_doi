#include "DerivationTree.h"
#include <stack>
#include <queue>

DerivationTree::DerivationTree(const std::string& startSymbol)
	: m_root(new Node(startSymbol))
{
	/* EMPTY */
}

DerivationTree::DerivationTree(const DerivationTree& derivationTree)
{
	*this = derivationTree;
}

DerivationTree::~DerivationTree()
{
	Clear();
}

bool DerivationTree::operator==(const DerivationTree& derivationTree) const
{
	auto thisCross = GetCross();
	auto thatCross = derivationTree.GetCross();

	if (thisCross.size() != thatCross.size()) {
		return false;
	}

	auto thisIt = thisCross.begin();
	auto thatIt = thatCross.begin();

	for (; thisIt != thisCross.end(); ++thisIt, ++thatIt) {
		if ((*thisIt)->GetSymbols() != (*thatIt)->GetSymbols()) {
			return false;
		}
	}
	return true;
}

DerivationTree& DerivationTree::operator=(const DerivationTree& derivationTree)
{
	if (!Empty()) {
		Clear();
	}
	std::queue<Node*> queue1;
	std::queue<Node*> queue2;
	queue1.push(derivationTree.GetRoot());
	m_root = new Node(derivationTree.GetRoot()->GetSymbols());

	Node* iterator;
	queue2.push(m_root);
	while (!queue1.empty()) {
		iterator = queue2.front();
		for (Node* node : queue1.front()->GetChildrens()) {
			queue1.push(node);
			Node* auxNode = new Node(node->GetSymbols());
			queue2.push(auxNode);
			iterator->AddChildren(auxNode);
		}
		queue1.pop();
		queue2.pop();
	}
	return *this;
}

bool DerivationTree::VerifyIfVectorContainsNode(const std::vector<Node*> vector, Node* node) const
{
	for (Node* toBeCheckedNode : vector) {
		if (toBeCheckedNode == node) {
			return true;
		}
	}
	return false;
}

unsigned int DerivationTree::GetLongestPath() const
{
	int currentLenght = 0;
	int maxLenght = 0;
	if (Empty()) {
		return maxLenght;
	}
	std::vector<Node*> visitedNodes;
	std::stack<Node*> stackOfNodes;
	stackOfNodes.push(m_root);

	while (!stackOfNodes.empty()) {
		if (!stackOfNodes.top()->HasChildrens()) {
			visitedNodes.push_back(stackOfNodes.top());
			stackOfNodes.pop();
			--currentLenght;
			continue;
		}

		bool allChildrensAreEvaluated = true;
		for (Node* node : stackOfNodes.top()->GetChildrens()) {
			if (VerifyIfVectorContainsNode(visitedNodes, node)) {
				continue;
			}
			allChildrensAreEvaluated = false;
			stackOfNodes.push(node);
			++currentLenght;
			if (currentLenght > maxLenght) {
				maxLenght = currentLenght;
			}
			break;
		}
		if (allChildrensAreEvaluated) {
			visitedNodes.push_back(stackOfNodes.top());
			stackOfNodes.pop();
			continue;
		}
	}
	return maxLenght;
}

DerivationTree::Node* DerivationTree::GetRoot() const
{
	return m_root;
}

std::vector<DerivationTree::Node*> DerivationTree::GetCross() const
{
	std::vector<Node*> result;
	if (Empty()) {
		return result;
	}
	std::stack<Node*> stackOfNodes;
	stackOfNodes.push(m_root);

	while (!stackOfNodes.empty()) {
		if (!stackOfNodes.top()->HasChildrens()) {
			result.push_back(stackOfNodes.top());
			stackOfNodes.pop();
			continue;
		}

		bool allChildrensAreEvaluated = true;
		for (Node* node : stackOfNodes.top()->GetChildrens()) {
			if (VerifyIfVectorContainsNode(result, node)) {
				continue;
			}
			allChildrensAreEvaluated = false;
			stackOfNodes.push(node);
			break;
		}
		if (allChildrensAreEvaluated) {
			result.push_back(stackOfNodes.top());
			stackOfNodes.pop();
			continue;
		}
	}
	return result;
}

std::vector<DerivationTree::Node*> DerivationTree::GetLeaves() const
{
	std::vector<Node*> result;
	for (Node* node : GetCross()) {
		if (!node->HasChildrens()) {
			result.push_back(node);
		}
	}
	return result;
}

std::string DerivationTree::GetResult() const
{
	std::string result;
	for (Node* node : GetLeaves()) {
		result += node->GetSymbols();
	}
	return result;
}

bool DerivationTree::Empty() const
{
	return m_root == nullptr;
}

void DerivationTree::Clear()
{
	for (Node* node : GetCross()) {
		delete node;
	}
	m_root = nullptr;
}

DerivationTree::Node::Node(const std::string& symbols, Node* parent)
	: m_symbols(symbols)
	, m_parent(parent)
{
	/* EMPTY */
}

const std::string& DerivationTree::Node::GetSymbols() const
{
	return m_symbols;
}

DerivationTree::Node* DerivationTree::Node::GetParent() const
{
	return m_parent;
}

const std::vector<DerivationTree::Node*>& DerivationTree::Node::GetChildrens() const
{
	return m_childrens;
}

bool DerivationTree::Node::HasChildrens() const
{
	return !m_childrens.empty();
}

void DerivationTree::Node::SetSymbols(const std::string& symbols)
{
	m_symbols = symbols;
}

void DerivationTree::Node::SetParent(Node* parent)
{
	m_parent = parent;
}

void DerivationTree::Node::SetChildrens(const std::vector<Node*>& childrens)
{
	m_childrens = childrens;
}

void DerivationTree::Node::AddChildren(Node* children)
{
	children->SetParent(this);
	m_childrens.push_back(children);
}

void DerivationTree::Node::ClearChildrens()
{
	for (size_t i = 0; i < m_childrens.size(); ++i) {
		delete m_childrens[i];
	}
	m_childrens.clear();
}

std::ostream& operator<<(std::ostream& out, const DerivationTree& derivationTree)
{
	auto cross = derivationTree.GetCross();
	for (const auto& node : cross) {
		out << node->GetSymbols() << ' ';
	}
	return out << '\n';
}
