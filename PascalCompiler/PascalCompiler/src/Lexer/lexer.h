#pragma once
#include "../IO/io.h"
#include "../utils/tokens.h"

class CLexer {
public:
	CLexer(Reader* reader);
	std::unique_ptr<CToken> getNextToken();
private:
	std::unique_ptr<Reader> reader;
	std::string getLetter() {

	}
};

