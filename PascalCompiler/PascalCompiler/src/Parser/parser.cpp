#include "parser.h"


CParser::CParser(CLexer* lexer) {
	this->lexer.reset(lexer);
}

void CParser::getNextToken() {
	token = lexer->getNextToken();
}