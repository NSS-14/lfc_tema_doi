#pragma once
#include <unordered_set>
#include <unordered_map>
#include <iostream>

class PushDownAutomaton
{
public:
	static const char kLambda = '_';

public:
	using StateStackSymbolPair = std::pair<std::string, std::string>;
	using DeltaResult = std::vector<StateStackSymbolPair>;
	using DeltaFunctionDefiniton = std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, DeltaResult>>>;

public:
	PushDownAutomaton();
	PushDownAutomaton(const PushDownAutomaton& pushDownAutomaton);

public:
	PushDownAutomaton& operator =(const PushDownAutomaton& pushDownAutomaton);
	bool operator ==(const PushDownAutomaton& pushDownAutomaton);
	friend std::ostream& operator <<(std::ostream& out, const PushDownAutomaton& pushDownAutomaton);

public:


private:
	std::unordered_set<std::string> m_states;
	std::unordered_set<std::string> m_alphabet;
	std::unordered_set<std::string> m_stackAlphabet;
	std::string m_initialState;
	std::string m_stackStartSymbol;
	std::unordered_set<std::string> m_finalStates;
	DeltaFunctionDefiniton m_delta;
};

