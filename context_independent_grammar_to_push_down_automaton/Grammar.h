#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <random>
#include <cstdint>
#include<unordered_map>

#include "DerivationTree.h"

class Grammar
{
public:
	enum class Type : uint8_t
	{
		Regular,
		ContextIndependent,
		ContextDependent,
		ZeroType,
		Invalid
	};

public:
	static const char kLambda = '_';

public:
	using Production = std::pair<std::string, std::string>;

public:
	Grammar();
	Grammar(std::ifstream& in);
	Grammar(const Grammar& grammar);

public:
	Grammar& operator=(const Grammar& grammar);
	bool operator==(const Grammar& grammar) const;
	friend std::ostream& operator<<(std::ostream& os, const Grammar& grammar); // 3 Print

public:
	const Type& GetType() const;

public:
	void ReadFile(std::ifstream& in); // 1 Read
	void Verify(); // 2 Verify
	bool VerifyVoidLanguage() const;

public:
	std::string GenerateWord() const; // 4 Generate
	std::vector<std::string> GenerateWords(int amount = 1) const; // 4 Generate

public:
	void PrintWord() const;
	void PrintWords(int amount = 1) const;

public:
	void SimplifyGrammar();
	void MakeItChomsky();
	void MakeItGreibach();

private:
	bool mf_VerifyIntersection() const;
	bool mf_VerifyStartSymbol() const;
	bool mf_VerifyAtLeastOneNonterminalInProductionLeft() const;
	bool mf_VerifyProductions() const;
	
private:
	bool mf_StringContainsAtLeastOneElementFromTheSet(const std::string& string, const std::unordered_set<char>& set) const;
	std::unordered_set<char> mf_ConvertVectorOfCharsToUset(const std::vector<char>& vector) const;
	std::vector<char> mf_ConvertUsetOfStringsToVectorOfChars(const std::unordered_set<std::string>& uSet) const;
	std::vector<int> mf_GetSubstrPositionsInString(const std::string& substr, const std::string& string) const;
	void mf_ApplyProductionOnString(int productionIndex, int positionInString, std::string& string) const;
	void mf_AddChildrensFromStringToNode(DerivationTree::Node* node, const std::string& string) const;
	bool mf_ContainsOnlyTerminals(const std::string& string) const;
	std::string mf_ConvertCharToSizeOneString(char character) const;
	std::unordered_set<std::string> mf_GetDifferenceBetweenCurrentNonterminalsAndNewNonterminals(const std::unordered_set<std::string>& newNonterminals) const;
	std::vector<size_t> mf_ReturnAllProductionIndexesThatHaveNonterminalInLeft(const std::string& nonterminal) const;
	bool mf_ContainsOnlyTerminalsOrTerminalsAndNonterminalsFromUset(const std::string& string, const std::unordered_set<std::string>& uSet) const;
	std::vector<std::string> mf_GetAllNonterminalsFromString(const std::string& string) const;
	int mf_GetRandom(const size_t& leftBound, const size_t& rightBound) const;
	std::string mf_ReturnNonTerminalThatGoesOnlyInTerminal(const std::vector<Production>& productions, char character) const;
	std::unordered_set<std::string> mf_ConvertProductionsLeftPartToUSet(const std::vector<Production>& productions) const;
	std::string mf_GetTheNextSymbolToBeAddedInProductions(const std::vector<Production>& productions, bool getZ = false) const;

private:
	void mf_RemoveUnusableNonterminals();
	void mf_RemoveUnaccesibleNonterminals();
	void mf_RemoveRenames();

private:
	void mf_ChomskyPartTwo();
	void mf_ChomskyPartThree();

private:
	void mf_GreibachPartOne();
	void mf_GreibachPartTwo();
	void mf_GreibachPartThree();

private:
	void mf_GreibachFirstLema(size_t productionIndex, size_t symbolFromRightPartIndex);
	void mf_GreibachSecondLema(std::vector<size_t> recursiveProductionsIndexes, std::vector<size_t> nonrecursiveProductionsIndexes);

private:
	std::vector<char> m_nonterminalSymbols;
	std::vector<char> m_terminalSymbols;
	char m_startSymbol;
	std::vector<Production> m_productions;
	Type m_type;
};
