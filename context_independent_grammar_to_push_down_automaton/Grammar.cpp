#include "Grammar.h"
#include "DerivationTree.h"
#include <set>

Grammar::Grammar()
	: m_startSymbol('\0')
	, m_type(Type::Invalid)
{
	/* EMPTY */
}
Grammar::Grammar(std::ifstream& in)
{
	ReadFile(in);
}
Grammar::Grammar(const Grammar& grammar)
{
	*this = grammar;
}

Grammar& Grammar::operator=(const Grammar& grammar)
{
	m_nonterminalSymbols = grammar.m_nonterminalSymbols;
	m_terminalSymbols = grammar.m_terminalSymbols;
	m_startSymbol = grammar.m_startSymbol;
	m_productions = grammar.m_productions;
	m_type = grammar.m_type;
	return *this;
}
bool Grammar::operator==(const Grammar& grammar) const
{
	return
		m_nonterminalSymbols == grammar.m_nonterminalSymbols
		&&
		m_terminalSymbols == grammar.m_terminalSymbols
		&&
		m_startSymbol == grammar.m_startSymbol
		&&
		m_productions == grammar.m_productions;
}
const Grammar::Type& Grammar::GetType() const
{
	return m_type;
}
std::ostream& operator<<(std::ostream& os, const Grammar& grammar)
{
	const auto& vn = grammar.m_nonterminalSymbols;
	const auto& vt = grammar.m_terminalSymbols;
	const auto& p = grammar.m_productions;
	std::vector<char>::const_iterator beforeEnd;

	os << "Vn: { ";
	beforeEnd = vn.size() > 1 ? vn.end() - 1 : vn.begin();
	for (auto it = vn.begin(); it != beforeEnd; ++it)
	{
		os << *it << ", ";
	}
	if (!vn.empty())
	{
		os << *beforeEnd << ' ';
	}
	os << '}' << '\n';

	os << "Vt: { ";
	beforeEnd = vt.size() > 1 ? vt.end() - 1 : vt.begin();
	for (auto it = vt.begin(); it != beforeEnd; ++it)
	{
		os << *it << ", ";
	}
	if (!vt.empty())
	{
		os << *beforeEnd << ' ';
	}
	os << '}' << '\n';

	os << "Start symbol: " << grammar.m_startSymbol << '\n';

	os << "Productions:" << '\n';
	for (size_t i = 0; i < p.size(); ++i)
	{
		os << p[i].first;
		os << " ---> ";
		os << p[i].second;
		os << '\n';
	}

	os << "Type: ";
	switch (grammar.m_type)
	{
	case Grammar::Type::Regular: os << "Regular"; break;
	case Grammar::Type::ContextIndependent: os << "Context independent"; break;
	case Grammar::Type::ContextDependent: os << "Context dependent"; break;
	case Grammar::Type::ZeroType: os << "Zero type"; break;
	default: os << "Invalid"; break;
	}
	os << '.' << '\n';
	return os;
}

void Grammar::ReadFile(std::ifstream& in)
{
	int vnSize;
	int vtSize;
	int productionsSize;
	char symbol;
	Production production;

	in >> vnSize;
	m_nonterminalSymbols.reserve(vnSize);
	for (int i = 0; i < vnSize; ++i)
	{
		in >> symbol;
		m_nonterminalSymbols.push_back(symbol);
	}

	in >> vtSize;
	m_terminalSymbols.reserve(vtSize);
	for (int i = 0; i < vtSize; ++i)
	{
		in >> symbol;
		m_terminalSymbols.push_back(symbol);
	}

	in >> m_startSymbol;

	in >> productionsSize;
	m_productions.reserve(productionsSize);
	for (int i = 0; i < productionsSize; ++i)
	{
		in >> production.first;
		in >> production.second;
		m_productions.push_back(production);
	}

	Verify();

	in.close();
}
void Grammar::Verify()
{
	if (!mf_VerifyIntersection())
	{
		m_type = Type::Invalid;
		return;
	}
	if (!mf_VerifyStartSymbol())
	{
		m_type = Type::Invalid;
		return;
	}
	if (!mf_VerifyAtLeastOneNonterminalInProductionLeft())
	{
		m_type = Type::Invalid;
		return;
	}
	if (!mf_VerifyProductions())
	{
		m_type = Type::Invalid;
		return;
	}
	for (const auto& production : m_productions) {
		if (production.first.size() > 1) {
			m_type = Type::ContextDependent;
			return;
		}
		switch (production.second.size())
		{
		case 1: {
			if (mf_ConvertVectorOfCharsToUset(m_nonterminalSymbols).count(production.second[0])) {
				m_type = Type::ContextIndependent;
				return;
			}
			break;
		}

		case 2: {
			std::unordered_set<char> nSet = mf_ConvertVectorOfCharsToUset(m_nonterminalSymbols);
			std::unordered_set<char> tSet = mf_ConvertVectorOfCharsToUset(m_terminalSymbols);
			if (nSet.count(production.second[0]) || tSet.count(production.second[1])) {
				m_type = Type::ContextIndependent;
				return;
			}
			break;
		}

		default: {
			m_type = Type::ContextIndependent;
			return;
		}
		}
	}
	m_type = Type::Regular;
}

std::string Grammar::GenerateWord() const
{
	if (m_type == Type::Invalid)
	{
		throw "The grammar hasn't passed all the tests.";
	}

	std::string currentWord;
	currentWord.push_back(m_startSymbol);

	std::vector<std::pair<int, std::vector<int>>> aplicableProductionsInCurrentWord;
	std::pair<int, std::vector<int>> pair;
	auto& [productionIndex, placesInCurrentWordWhereItCanBeApplied] = pair;

	while (true)
	{
		for (int i = 0; i < m_productions.size(); ++i)
		{
			placesInCurrentWordWhereItCanBeApplied = mf_GetSubstrPositionsInString(m_productions[i].first, currentWord);
			if (!placesInCurrentWordWhereItCanBeApplied.empty())
			{
				productionIndex = i;
				aplicableProductionsInCurrentWord.push_back(pair);
			}
		}
		if (aplicableProductionsInCurrentWord.empty())
		{
			break;
		}
		const size_t& sizeOfApplicableProductions = aplicableProductionsInCurrentWord.size();
		size_t randomApplicableProduction = mf_GetRandom(0, sizeOfApplicableProductions - 1);

		int productionIndex = aplicableProductionsInCurrentWord[randomApplicableProduction].first;

		const size_t& sizeOfPlacesWhereThisProductionCanBeApplied = aplicableProductionsInCurrentWord[randomApplicableProduction].second.size();
		size_t randomPositionInVectorOfPlaces = mf_GetRandom(0, sizeOfPlacesWhereThisProductionCanBeApplied - 1);

		int positionInString = aplicableProductionsInCurrentWord[randomApplicableProduction].second[randomPositionInVectorOfPlaces];

		mf_ApplyProductionOnString(productionIndex, positionInString, currentWord);
		aplicableProductionsInCurrentWord.clear();
	}
	return currentWord;
}
std::vector<std::string> Grammar::GenerateWords(int amount) const
{
	std::vector<std::string> result;
	result.reserve(amount);

	for (int i = 0; i < amount; ++i)
	{
		result.push_back(GenerateWord());
	}
	return result;
}
void Grammar::PrintWord() const
{
	std::cout << GenerateWord();
}
void Grammar::PrintWords(int amount) const
{
	std::vector<std::string> words;
	words = GenerateWords(amount);
	for (const std::string& word : words)
	{
		std::cout << word << '\n';
	}
}

bool Grammar::mf_VerifyIntersection() const
{
	std::unordered_set<char> nonterminals;
	nonterminals = mf_ConvertVectorOfCharsToUset(m_nonterminalSymbols);

	for (char symbol : m_terminalSymbols)
	{
		if (nonterminals.count(symbol))
		{
			return false;
		}
	}
	return true;
}
bool Grammar::mf_VerifyStartSymbol() const
{
	for (char symbol : m_nonterminalSymbols)
	{
		if (symbol == m_startSymbol)
		{
			return true;
		}
	}
	return false;
}
bool Grammar::mf_VerifyAtLeastOneNonterminalInProductionLeft() const
{
	std::unordered_set<char> nonterminals;
	nonterminals = mf_ConvertVectorOfCharsToUset(m_nonterminalSymbols);

	for (const Production& production : m_productions)
	{
		if (!mf_StringContainsAtLeastOneElementFromTheSet(production.first, nonterminals))
		{
			return false;
		}
	}
	return true;
}
bool Grammar::mf_VerifyProductions() const
{
	std::unordered_set<char> nonterminals;
	std::unordered_set<char> terminals;

	nonterminals = mf_ConvertVectorOfCharsToUset(m_nonterminalSymbols);
	terminals = mf_ConvertVectorOfCharsToUset(m_terminalSymbols);

	for (const Production& production : m_productions)
	{
		for (char character : production.first)
		{
			if (!nonterminals.count(character) && !terminals.count(character) && character != kLambda)
			{
				return false;
			}
		}
		for (char character : production.second)
		{
			if (!nonterminals.count(character) && !terminals.count(character) && character != kLambda)
			{
				return false;
			}
		}
	}
	return true;
}

bool Grammar::VerifyVoidLanguage() const
{
	if (m_type == Type::ContextDependent || m_type == Type::ZeroType || m_type == Type::Invalid) {
		return false;
	}
	std::vector<DerivationTree> backtracking;
	backtracking.reserve(5000);
	size_t currentGenerationIndex = 0;

	std::string startSymbol;
	startSymbol.push_back(m_startSymbol);
	backtracking.push_back(DerivationTree(startSymbol));
	size_t nextGenerationIndex = backtracking.size();

	auto nonterminalsSet = mf_ConvertVectorOfCharsToUset(m_nonterminalSymbols);

	while (true) {
		for (size_t i = currentGenerationIndex; i < nextGenerationIndex; ++i) {
			std::vector<std::pair<DerivationTree::Node*, std::vector<size_t>>> aplicableProductionOnNonterminalNodes;
			for (DerivationTree::Node* node : backtracking[i].GetLeaves()) {
				if (nonterminalsSet.count(node->GetSymbols()[0])) {
					std::vector<size_t> aplicableProductionOnCurrentNode;
					for (size_t i = 0; i < m_productions.size(); ++i) {
						if (m_productions[i].first[0] == node->GetSymbols()[0]) {
							aplicableProductionOnCurrentNode.push_back(i);
						}
					}
					aplicableProductionOnNonterminalNodes.emplace_back(node, aplicableProductionOnCurrentNode);
				}
			}
			for (const auto& pair : aplicableProductionOnNonterminalNodes) {
				auto& [node, productionIndexes] = pair;
				for (const auto& index : productionIndexes) {
					mf_AddChildrensFromStringToNode(node, m_productions[index].second);
					if (backtracking[i].GetLongestPath() <= m_nonterminalSymbols.size()) {
						backtracking.push_back(backtracking[i]);
					}
					node->ClearChildrens();
				}
			}
		}
		currentGenerationIndex = nextGenerationIndex;
		nextGenerationIndex = backtracking.size();
		if (currentGenerationIndex == nextGenerationIndex) {
			break;
		}
	}

	for (const auto& tree : backtracking) {
		if (mf_ContainsOnlyTerminals(tree.GetResult())) {
			return true;
		}
	}
	return false;
}

bool Grammar::mf_StringContainsAtLeastOneElementFromTheSet(const std::string& string, const std::unordered_set<char>& set) const
{
	for (char character : string)
	{
		if (set.count(character))
		{
			return true;
		}
	}
	return false;
}
std::unordered_set<char> Grammar::mf_ConvertVectorOfCharsToUset(const std::vector<char>& vector) const
{
	std::unordered_set<char> result;
	for (char character : vector)
	{
		result.insert(character);
	}
	return result;
}
std::vector<char> Grammar::mf_ConvertUsetOfStringsToVectorOfChars(const std::unordered_set<std::string>& uSet) const
{
	std::vector<char> result;
	for (const auto& character : uSet) {
		result.push_back(character[0]);
	}
	return result;
}
std::vector<int> Grammar::mf_GetSubstrPositionsInString(const std::string& substr, const std::string& string) const
{
	std::vector<int> result;
	if (substr.size() > string.size())
	{
		return result;
	}
	if (substr.empty())
	{
		return result;
	}
	for (int i = 0; i < string.size() - substr.size() + 1; ++i)
	{
		if (string.substr(i, substr.size()) == substr)
		{
			result.push_back(i);
		}
	}
	return result;
}
void Grammar::mf_ApplyProductionOnString(int productionIndex, int positionInString, std::string& string) const
{
	const Production& appliedProduction = m_productions[productionIndex];
	size_t differenceInSizes = appliedProduction.second.size() - appliedProduction.first.size();

	if (!differenceInSizes)
	{
		for (int i = 0; i < appliedProduction.second.size(); ++i)
		{
			string[positionInString + i] = appliedProduction.second[i];
		}
		return;
	}

	std::string newString;
	newString.reserve(string.size() + differenceInSizes);

	for (int i = 0; i < positionInString; ++i)
	{
		newString.push_back(string[i]);
	}
	for (int i = 0; i < appliedProduction.second.size(); ++i)
	{
		newString.push_back(appliedProduction.second[i]);
	}
	for (size_t i = positionInString + appliedProduction.first.size(); i < string.size(); ++i)
	{
		newString.push_back(string[i]);
	}
	string = newString;
}

void Grammar::mf_AddChildrensFromStringToNode(DerivationTree::Node* node, const std::string& string) const
{
	for (auto character : string) {
		std::string aux;
		aux.push_back(character);
		node->AddChildren(new DerivationTree::Node(aux));
	}
}

bool Grammar::mf_ContainsOnlyTerminals(const std::string& string) const
{
	auto vt = mf_ConvertVectorOfCharsToUset(m_terminalSymbols);
	for (char character : string) {
		if (!vt.count(character)) {
			return false;
		}
	}
	return true;
}

std::string Grammar::mf_ConvertCharToSizeOneString(char character) const
{
	std::string result;
	result.push_back(character);
	return result;
}

std::unordered_set<std::string> Grammar::mf_GetDifferenceBetweenCurrentNonterminalsAndNewNonterminals(const std::unordered_set<std::string>& newNonterminals) const
{
	std::unordered_set<std::string> result;
	for (char character : m_nonterminalSymbols) {
		if (!newNonterminals.count(mf_ConvertCharToSizeOneString(character))) {
			result.insert(mf_ConvertCharToSizeOneString(character));
		}
	}
	return result;
}

std::vector<size_t> Grammar::mf_ReturnAllProductionIndexesThatHaveNonterminalInLeft(const std::string& nonterminal) const
{
	std::vector<size_t> result;
	for (size_t i = 0; i < m_productions.size(); ++i) {
		if (m_productions[i].first[0] == nonterminal[0]) {
			result.push_back(i);
		}
	}
	return result;
}

void Grammar::SimplifyGrammar()
{
	if (m_type != Type::ContextIndependent && m_type != Type::Regular) {
		return;
	}
	if (!VerifyVoidLanguage()) {
		return;
	}
	mf_RemoveRenames();
	mf_RemoveUnusableNonterminals();
	mf_RemoveUnaccesibleNonterminals();
}

void Grammar::MakeItChomsky()
{
	mf_ChomskyPartTwo();
	mf_ChomskyPartThree();
}
void Grammar::MakeItGreibach()
{
	mf_GreibachPartOne();
	mf_GreibachPartTwo();
	mf_GreibachPartThree();
}

void Grammar::mf_RemoveUnusableNonterminals()
{
	std::unordered_set<std::string> newNonterminals;
	while (true)
	{
		std::unordered_set<std::string> currentNewNonterminals = newNonterminals;
		for (const auto& production : m_productions) {
			if (mf_ContainsOnlyTerminalsOrTerminalsAndNonterminalsFromUset(production.second, newNonterminals)) {
				currentNewNonterminals.insert(production.first);
			}
		}
		if (currentNewNonterminals == newNonterminals) {
			break;
		}
		newNonterminals = currentNewNonterminals;
	}
	std::unordered_set<std::string> toBeRemovedNonterminals = mf_GetDifferenceBetweenCurrentNonterminalsAndNewNonterminals(newNonterminals);
	std::vector<Production> newProductions;
	for (const auto& production : m_productions) {
		bool productionIsGood = true;
		for (char character : production.first) {
			if (toBeRemovedNonterminals.count(mf_ConvertCharToSizeOneString(character))) {
				productionIsGood = false;
				break;
			}
		}
		if (!productionIsGood) {
			continue;
		}
		for (char character : production.second) {
			if (toBeRemovedNonterminals.count(mf_ConvertCharToSizeOneString(character))) {
				productionIsGood = false;
				break;
			}
		}
		if (productionIsGood) {
			newProductions.push_back(production);
		}
	}
	m_productions = newProductions;
	m_nonterminalSymbols.clear();
	m_nonterminalSymbols.reserve(newNonterminals.size());
	for (const auto& string : newNonterminals) {
		m_nonterminalSymbols.push_back(string[0]);
	}
}
void Grammar::mf_RemoveUnaccesibleNonterminals()
{
	std::unordered_set<std::string> newNonterminals;
	newNonterminals.insert(mf_ConvertCharToSizeOneString(m_startSymbol));
	while (true)
	{
		std::unordered_set<std::string> currentNewNonterminals = newNonterminals;
		for (const auto& production : m_productions)
		{
			if (currentNewNonterminals.count(production.first))
			{
				for (const auto& string : mf_GetAllNonterminalsFromString(production.second)) {
					currentNewNonterminals.insert(string);
				}
			}
		}
		if (currentNewNonterminals == newNonterminals)
		{
			break;
		}
		newNonterminals = currentNewNonterminals;
	}
	std::unordered_set<std::string> toBeRemovedNonterminals = mf_GetDifferenceBetweenCurrentNonterminalsAndNewNonterminals(newNonterminals);
	std::vector<Production> newProductions;
	for (const auto& production : m_productions) {
		bool productionIsGood = true;
		for (char character : production.first) {
			if (toBeRemovedNonterminals.count(mf_ConvertCharToSizeOneString(character))) {
				productionIsGood = false;
				break;
			}
		}
		if (!productionIsGood) {
			continue;
		}
		for (char character : production.second) {
			if (toBeRemovedNonterminals.count(mf_ConvertCharToSizeOneString(character))) {
				productionIsGood = false;
				break;
			}
		}
		if (productionIsGood) {
			newProductions.push_back(production);
		}
	}
	m_productions = newProductions;
	m_nonterminalSymbols.clear();
	m_nonterminalSymbols.reserve(newNonterminals.size());
	for (const auto& string : newNonterminals) {
		m_nonterminalSymbols.push_back(string[0]);
	}
}
void Grammar::mf_RemoveRenames()
{
	auto vn = mf_ConvertVectorOfCharsToUset(m_nonterminalSymbols);
	std::vector<Production> newProductions;
	std::unordered_set<size_t> renames;
	for (size_t i = 0; i < m_productions.size(); ++i) {
		const auto& rightPart = m_productions[i].second;
		if (rightPart.size() == 1 && vn.count(rightPart[0])) {
			renames.insert(i);
		}
	}
	for (size_t i = 0; i < m_productions.size(); ++i) {
		if (renames.count(i)) {
			continue;
		}
		newProductions.push_back(m_productions[i]);
	}
	for (size_t renameIndex : renames) {
		for (size_t productionIndex : mf_ReturnAllProductionIndexesThatHaveNonterminalInLeft(m_productions[renameIndex].second)) {
			newProductions.emplace_back(m_productions[renameIndex].first, m_productions[productionIndex].second);
		}
	}
	m_productions = newProductions;
}

void Grammar::mf_ChomskyPartTwo()
{
	std::vector<Production> newProductions;
	std::unordered_set<size_t> productionsIndexesThatHaveMoreThanOneInRightPart;

	auto vn = mf_ConvertVectorOfCharsToUset(m_nonterminalSymbols);
	auto vt = mf_ConvertVectorOfCharsToUset(m_terminalSymbols);

	for (size_t i = 0; i < m_productions.size(); ++i) {
		if (m_productions[i].second.size() > 1) {
			productionsIndexesThatHaveMoreThanOneInRightPart.insert(i);
			continue;
		}
		newProductions.push_back(m_productions[i]);
	}
	for (size_t index : productionsIndexesThatHaveMoreThanOneInRightPart) {
		for (size_t i = 0; i < m_productions[index].second.size(); ++i) {
			auto& currentCharacter = m_productions[index].second[i];
			if (vt.count(currentCharacter)) {
				auto& nts = m_nonterminalSymbols;
				auto nonterminalThatAlreadyExists = mf_ReturnNonTerminalThatGoesOnlyInTerminal(newProductions, currentCharacter);
				if (nonterminalThatAlreadyExists.size()) {
					currentCharacter = nonterminalThatAlreadyExists[0];
					continue;
				}
				std::string nextNonterminal = mf_GetTheNextSymbolToBeAddedInProductions(newProductions);
				nts.push_back(nextNonterminal[0]);
				newProductions.emplace_back(nextNonterminal, mf_ConvertCharToSizeOneString(currentCharacter));
				currentCharacter = nextNonterminal[0];
			}
		}
		newProductions.push_back(m_productions[index]);
	}
	m_productions = newProductions;
}

void Grammar::mf_ChomskyPartThree()
{
	std::vector<Production>newProductions;
	std::vector<size_t>productionsThatHaveMoreThanTwoInRightIndexes;
	for (size_t i = 0; i < m_productions.size(); ++i) {
		if (m_productions[i].second.size() > 2) {
			productionsThatHaveMoreThanTwoInRightIndexes.push_back(i);
			continue;
		}
		newProductions.push_back(m_productions[i]);
	}
	for (size_t index : productionsThatHaveMoreThanTwoInRightIndexes) {
		std::string currentRightPartOfProduction = m_productions[index].second;
		std::string lastCreatedNonTerminal = mf_GetTheNextSymbolToBeAddedInProductions(newProductions);
		m_nonterminalSymbols.push_back(lastCreatedNonTerminal[0]);
		m_productions[index].second.resize(2);
		m_productions[index].second[1] = lastCreatedNonTerminal[0];
		newProductions.push_back(m_productions[index]);
		for (size_t i = 1; i < currentRightPartOfProduction.size() - 2; ++i) {
			std::string newNonTerminal = mf_GetTheNextSymbolToBeAddedInProductions(newProductions);
			m_nonterminalSymbols.push_back(newNonTerminal[0]);
			newNonTerminal.push_back(currentRightPartOfProduction[i]);
			std::swap(newNonTerminal[0], newNonTerminal[1]);
			newProductions.emplace_back(lastCreatedNonTerminal, newNonTerminal);
			std::swap(newNonTerminal[0], newNonTerminal[1]);
			newNonTerminal.resize(1);
			lastCreatedNonTerminal = newNonTerminal;
		}
		newProductions.emplace_back(lastCreatedNonTerminal, currentRightPartOfProduction.substr(currentRightPartOfProduction.size() - 2));
	}
	m_productions = newProductions;
}

void Grammar::mf_GreibachPartOne()
{
	std::vector<Production> beforeModificationProductions = m_productions;
	while (true) {
		std::vector<size_t> recursiveProductions;
		std::vector<size_t> nonrecursiveProductions;
		char* currentScopedSymbol = nullptr;
		bool applyFirstLema = true;
		for (int i = 0; i < m_productions.size(); ++i) {
			auto vt = mf_ConvertVectorOfCharsToUset(m_terminalSymbols);
			if (m_productions[i].first[0] == m_startSymbol || (m_productions[i].first[0] >= 33 && m_productions[i].first[0] <= 64)) {
				continue;
			}
			if (vt.count(m_productions[i].second[0])) {
				continue;
			}
			if (currentScopedSymbol == nullptr) {
				currentScopedSymbol = new char(m_productions[i].first[0]);
				--i;
				continue;
			}
			if (m_productions[i].first[0] == *currentScopedSymbol) {
				if (applyFirstLema) {
					if (m_productions[i].first[0] == m_productions[i].second[0]) {
						applyFirstLema = false;
						--i;
						continue;
					}
					if (!vt.count(m_productions[i].second[0])) {
						mf_GreibachFirstLema(i, 0);
						--i;
					}
				}
				if (m_productions[i].first[0] == m_productions[i].second[0]) {
					recursiveProductions.push_back(i);
				}
			}
		}
		if (!applyFirstLema) {
			for (int i = 0; i < m_productions.size(); ++i) {
				if (m_productions[i].first[0] == *currentScopedSymbol && m_productions[i].second.size() == 1) {
					nonrecursiveProductions.push_back(i);
				}
			}
			mf_GreibachSecondLema(recursiveProductions, nonrecursiveProductions);
		}
		delete currentScopedSymbol;
		if (beforeModificationProductions == m_productions) {
			break;
		}
		beforeModificationProductions = m_productions;
	}
}
void Grammar::mf_GreibachPartTwo()
{
	auto vn = mf_ConvertVectorOfCharsToUset(m_nonterminalSymbols);
	for (size_t i = 0; i < m_productions.size(); ++i) {
		if(m_productions[i].first[0]==m_startSymbol && m_productions[i].second.size()>1 && vn.count(m_productions[i].second[0])) {
			mf_GreibachFirstLema(i, 0);
		}
	}
}
void Grammar::mf_GreibachPartThree()
{
	auto vn = mf_ConvertVectorOfCharsToUset(m_nonterminalSymbols);
	for (size_t i = 0; i < m_productions.size(); ++i) {
		if ((m_productions[i].first[0] >= 33 && m_productions[i].first[0]<=64) &&
			vn.count(m_productions[i].second[0])) {
			mf_GreibachFirstLema(i, 0);
		}
	}
}

void Grammar::mf_GreibachFirstLema(size_t productionIndex, size_t symbolFromRightPartIndex)
{
	char symbolToBeReplaced = m_productions[productionIndex].second[symbolFromRightPartIndex];
	bool firstModyfication = false;
	std::string initialRightPartForm = m_productions[productionIndex].second;
	for (size_t i = 0; i < m_productions.size(); ++i) {
		if (m_productions[i].first == mf_ConvertCharToSizeOneString(symbolToBeReplaced)) {
			if (!firstModyfication) {
				mf_ApplyProductionOnString(i, symbolFromRightPartIndex, m_productions[productionIndex].second);
				firstModyfication = true;
				continue;
			}
			std::string newRightPart = initialRightPartForm;
			mf_ApplyProductionOnString(i, symbolFromRightPartIndex, newRightPart);
			m_productions.emplace_back(m_productions[productionIndex].first, newRightPart);
		}
	}
}
void Grammar::mf_GreibachSecondLema(std::vector<size_t> recursiveProductionsIndexes, std::vector<size_t> nonrecursiveProductionsIndexes)
{
	std::vector<Production>newProductions;
	for (size_t nonrecursiveIndex : nonrecursiveProductionsIndexes) {
		std::string newZNonTerminal = mf_GetTheNextSymbolToBeAddedInProductions(newProductions, true);
		m_nonterminalSymbols.push_back(newZNonTerminal[0]);
		newProductions.emplace_back(m_productions[nonrecursiveIndex].first, m_productions[nonrecursiveIndex].second + newZNonTerminal);
		newProductions.push_back(m_productions[nonrecursiveIndex]);
		for (size_t recursiveIndex : recursiveProductionsIndexes) {
			std::string rightPartWithoutFirstCharcter = m_productions[recursiveIndex].second.substr(1);
			newProductions.emplace_back(newZNonTerminal, rightPartWithoutFirstCharcter);
			newProductions.emplace_back(newZNonTerminal, rightPartWithoutFirstCharcter + newZNonTerminal);
		}
	}
	std::unordered_set<size_t>recursiveUset(std::begin(recursiveProductionsIndexes), std::end(recursiveProductionsIndexes));
	std::unordered_set<size_t>nonrecursiveUset(std::begin(nonrecursiveProductionsIndexes), std::end(nonrecursiveProductionsIndexes));
	for (size_t i = 0; i < m_productions.size(); ++i) {
		if (recursiveUset.count(i) || nonrecursiveUset.count(i)) {
			continue;
		}
		newProductions.push_back(m_productions[i]);
	}
	m_productions = newProductions;
}

bool Grammar::mf_ContainsOnlyTerminalsOrTerminalsAndNonterminalsFromUset(const std::string& string, const std::unordered_set<std::string>& uSet) const
{
	auto vt = mf_ConvertVectorOfCharsToUset(m_terminalSymbols);
	for (char character : string) {
		if (!vt.count(character) && !uSet.count(mf_ConvertCharToSizeOneString(character))) {
			return false;
		}
	}
	return true;
}

std::vector<std::string> Grammar::mf_GetAllNonterminalsFromString(const std::string& string) const
{
	std::vector<std::string> result;
	auto vn = mf_ConvertVectorOfCharsToUset(m_nonterminalSymbols);
	for (char character : string)
	{
		if (vn.count(character)) {
			result.push_back(mf_ConvertCharToSizeOneString(character));
		}
	}
	return result;
}

int Grammar::mf_GetRandom(const size_t& leftBound, const size_t& rightBound) const
{
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(leftBound, rightBound);
	return distr(eng);
}

std::string Grammar::mf_ReturnNonTerminalThatGoesOnlyInTerminal(const std::vector<Production>& productions, char character) const
{
	std::vector<Production> allProductionsThatGoesIntoCharacter;
	std::unordered_map<std::string, unsigned int> nonTerminalApparitions;

	for (const auto& production : productions) {
		if (nonTerminalApparitions.count(production.first)) {
			++nonTerminalApparitions[production.first];
		}
		else {
			nonTerminalApparitions[production.first] = 1;
		}
		if (production.second.size() == 1) {
			if (production.second[0] == character) {
				allProductionsThatGoesIntoCharacter.push_back(production);
			}
		}
	}
	for (const auto& production : m_productions) {
		if (nonTerminalApparitions.count(production.first)) {
			++nonTerminalApparitions[production.first];
		}
		else {
			nonTerminalApparitions[production.first] = 1;
		}
		if (production.second.size() == 1) {
			if (production.second[0] == character) {
				allProductionsThatGoesIntoCharacter.push_back(production);
			}
		}
	}
	for (const auto& production : allProductionsThatGoesIntoCharacter) {
		if (nonTerminalApparitions[production.first] == 1) {
			return production.first;
		}
	}
	return {};
}

std::unordered_set<std::string> Grammar::mf_ConvertProductionsLeftPartToUSet(const std::vector<Production>& productions) const
{
	std::unordered_set<std::string> result;
	for (const auto& production : productions) {
		result.insert(production.first);
	}
	return result;
}

std::string Grammar::mf_GetTheNextSymbolToBeAddedInProductions(const std::vector<Production>& productions, bool getZ) const
{
	auto leftPartOfProductions = mf_ConvertProductionsLeftPartToUSet(productions);
	for (const auto& production : m_productions) {
		leftPartOfProductions.insert(production.first);
	}

	if (getZ) {
		for (char character = 33; character <= 64; ++character) {
			if (leftPartOfProductions.count(mf_ConvertCharToSizeOneString(character))) {
				continue;
			}
			return mf_ConvertCharToSizeOneString(character);
		}
	}
	else {
		for (char character = 'A'; character <= 'Z'; ++character) {
			if (leftPartOfProductions.count(mf_ConvertCharToSizeOneString(character))) {
				continue;
			}
			return mf_ConvertCharToSizeOneString(character);
		}
	}
	return {};
}
