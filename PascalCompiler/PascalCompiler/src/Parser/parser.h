#pragma once
#include "../Lexer/lexer.h"

class CParser {
public:
	CParser(CLexer* lexer);
	void getNextToken();

private:
	std::unique_ptr<CToken> token;
	std::unique_ptr<CLexer> lexer;
};