#pragma once
#include "../IO/io.h"
#include "../utils/tokens.h"
#include "../utils/variants.h"
#include "../utils/utils.h"
#include "../utils/error.h"

class CLexer {
public:
	CLexer(Reader* reader);
	std::unique_ptr<CToken> getNextToken();
	void getNextChar();

private:
	std::unique_ptr<Reader> reader;
	char ch;
	TextPosition pos;

	std::string getName();
	std::pair<VariantType, std::string> getNumber();
	void deleteComment();

	bool isSpace(char ch);
	bool isLetter(char ch);
	bool isDigit(char ch);
	bool isNumber(std::string &number);
	
};

