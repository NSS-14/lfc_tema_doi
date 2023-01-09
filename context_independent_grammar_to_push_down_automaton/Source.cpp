#include "Grammar.h"
#include <iostream>

int main()
{
	Grammar g;

	std::ifstream in("grammar_input.txt");
	g.ReadFile(in);
	g.SimplifyGrammar();
	g.MakeItChomsky();
	//g.MakeItGraibach();
	std::cout << g;

	return 0;
}