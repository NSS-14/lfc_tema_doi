#include "PushDownAutomaton.h"

PushDownAutomaton::PushDownAutomaton()
{
	/* EMPTY */
}
PushDownAutomaton::PushDownAutomaton(const PushDownAutomaton& pushDownAutomaton)
{
	*this = pushDownAutomaton;
}

PushDownAutomaton& PushDownAutomaton::operator=(const PushDownAutomaton& pushDownAutomaton)
{
	m_states = pushDownAutomaton.m_states;
	m_alphabet = pushDownAutomaton.m_alphabet;
	m_stackAlphabet = pushDownAutomaton.m_stackAlphabet;
	m_initialState = pushDownAutomaton.m_initialState;
	m_stackStartSymbol = pushDownAutomaton.m_stackStartSymbol;
	m_finalStates = pushDownAutomaton.m_finalStates;
	m_delta = pushDownAutomaton.m_delta;
	return *this;
}

bool PushDownAutomaton::operator==(const PushDownAutomaton& pushDownAutomaton)
{
	return m_states == pushDownAutomaton.m_states
		&& m_alphabet == pushDownAutomaton.m_alphabet
		&& m_stackAlphabet == pushDownAutomaton.m_stackAlphabet
		&& m_initialState == pushDownAutomaton.m_initialState
		&& m_stackStartSymbol == pushDownAutomaton.m_stackStartSymbol
		&& m_finalStates == pushDownAutomaton.m_finalStates
		&& m_delta == pushDownAutomaton.m_delta;
}

std::ostream& operator<<(std::ostream& out, const PushDownAutomaton& pushDownAutomaton)
{
	const auto& states = pushDownAutomaton.m_states;
	const auto& alphabet = pushDownAutomaton.m_alphabet;
	const auto& stackAlphabet = pushDownAutomaton.m_stackAlphabet;
	const auto& initialState = pushDownAutomaton.m_initialState;
	const auto& stackStartSymbol = pushDownAutomaton.m_stackStartSymbol;
	const auto& finalStates = pushDownAutomaton.m_finalStates;
	const auto& delta = pushDownAutomaton.m_delta;
	size_t size;

	out << "States: { ";
	size = states.size();
	if (size) {
		auto it = states.begin();
		for (size_t i = 0; i < size - 1; ++i)
		{
			out << *it << ", ";
			++it;
		}
		out << *it << " }";
	}
	else {
		out << '}';
	}
	out << '\n';

	out << "Alphabet: { ";
	size = alphabet.size();
	if (size) {
		auto it = alphabet.begin();
		for (size_t i = 0; i < size - 1; ++i)
		{
			out << *it << ", ";
			++it;
		}
		out << *it << " }";
	}
	else {
		out << '}';
	}
	out << '\n';

	out << "Stack alphabet: { ";
	size = stackAlphabet.size();
	if (size) {
		auto it = stackAlphabet.begin();
		for (size_t i = 0; i < size - 1; ++i)
		{
			out << *it << ", ";
			++it;
		}
		out << *it << " }";
	}
	else {
		out << '}';
	}
	out << '\n';

	out << "Initial state: " << initialState;
	out << '\n';

	out << "Stack start symbol: " << stackStartSymbol;
	out << '\n';

	out << "Final states: { ";
	size = finalStates.size();
	if (size) {
		auto it = finalStates.begin();
		for (size_t i = 0; i < size - 1; ++i)
		{
			out << *it << ", ";
			++it;
		}
		out << *it << " }";
	}
	else {
		out << '}';
	}
	out << '\n';

	out << "Transitions:" << '\n';
	for (const auto& mainMaps : delta) {
		const auto& [state, secondMaps] = mainMaps;
		for (const auto& secondMap : secondMaps) {
			const auto& [stackSymbol, thirdMaps] = secondMap;
			for (const auto& thirdMap : thirdMaps) {
				const auto& [alphabetSymbol, vectorOfPairs] = thirdMap;
				out << '(' << state << ", " << stackSymbol << ", " << alphabetSymbol << ')';
				out << " = ";
				out << "{\n";
				for (const auto& pair : vectorOfPairs) {
					out << '(' << pair.first << ", " << pair.second << ')' << '\n';
				}
				out << "}\n";
			}
		}
	}
	return out;
}
