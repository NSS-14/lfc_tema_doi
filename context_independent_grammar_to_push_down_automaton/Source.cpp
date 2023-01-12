//#include "Grammar.h"
#include "PushDownAutomaton.h"
#include <iostream>

int main()
{
	/*Grammar g;

	std::ifstream in("grammar_input.txt");
	g.ReadFile(in);
	g.SimplifyGrammar();
	g.MakeItChomsky();
	g.MakeItGreibach();*/

	PushDownAutomaton pda;
	std::cout << pda;

	return 0;
}