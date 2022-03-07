#include "lexer.h"


CLexer::CLexer(Reader* reader) {
	this->reader.reset(reader);
	std::tie(ch, pos) = reader->get();
}

void CLexer::getNextChar() {
	std::tie(ch, pos) = reader->get();
}

bool CLexer::isSpace(char ch) {
	return isspace(ch);
}

bool CLexer::isLetter(char ch) {
	return ch >= 'A' && ch <= 'Z' || ch >= 'a' && ch <= 'z' || ch == '_';
}

bool CLexer::isDigit(char ch) {
	return isdigit(ch);
}


std::string CLexer::getName() {
	std::string name;
	while (isLetter(ch) || isDigit(ch)) {
		name += ch;
		getNextChar();
	}

	return name;
}


bool CLexer::isNumber(std::string& number) {
	int cntDots = 0;
	for (const char& c : number) {
		if (isLetter(c))
			return false;
		if (c == '.')
			cntDots++;
	}
	return cntDots < 2;
}


std::pair<VariantType, std::string> CLexer::getNumber() {

	TextPosition startPos = pos;

	std::string number;
	while (isDigit(ch) || isLetter(ch) || ch == '.') {
		number += ch;
		getNextChar();
	}

	if (!isNumber(number))
		throw Error(ErrorCodes::IncorrectNumber, startPos, number);

	if (count(number.begin(), number.end(), '.')) {
		//overflow for real type?
		return std::make_pair(VariantType::vtReal, number);
	}
	else {
		if (size(number) > MAX_INT_LEN)
			throw Error(ErrorCodes::IntegerOverflow, startPos, number);
		return std::make_pair(VariantType::vtInt, number);
	}
}


void CLexer::deleteComment() {
	TextPosition startPos = pos;

	while (ch != '}' && ch != EOF)
		getNextChar();

	if (ch == EOF)
		throw Error(ErrorCodes::NoMatchingSymbol, startPos, "{");
	else
		getNextChar();
}

std::string CLexer::getString() {
	TextPosition startPos = pos;

	std::string str;
	getNextChar();

	while (ch != '\n') {
		str += ch;
		getNextChar();
		if (ch == '\'') {
			getNextChar();
			if (ch != '\'')
				break;
		}
	}

	if (ch == '\n')
		throw Error(ErrorCodes::NoMatchingSymbol, startPos, "'");
	else {
		return str;
	}
}

std::unique_ptr<CToken> CLexer::getNextToken() {

	while (isSpace(ch) || ch == '{') { //comments
		if (ch == '{')
			deleteComment();
		else
			getNextChar();
	}

	if (ch == EOF)
		return nullptr;

	TextPosition startPos = pos;
	
	if (ch == '}')
		throw Error(ErrorCodes::UnexpectedSymbol, startPos, "}");


	if (isLetter(ch)) { //ident, keywords, true, false
		std::string name = getName();
		std::unique_ptr<CToken> token;

		if (isKeyword(name)) {
			token = std::make_unique<CKeywordToken>(strToKeywords.at(name), startPos);
		}
		else
			if (name == "true" || name == "false") {
				bool value = (name == "true" ? true : false);
				std::unique_ptr<CVariant> boolVariant = std::make_unique<CBooleanVariant>(value);
				token = std::make_unique<CConstToken>(boolVariant.release(), startPos);
			}
			else {
				token = std::make_unique<CIdentToken>(name, startPos);
			}

		return token;
	}

	if (ch == '\'') { //string
		std::string value = getString();
		std::unique_ptr<CVariant> stringVariant = std::make_unique<CStringVariant>(value);
		return std::make_unique<CConstToken>(stringVariant.release(), startPos);
	}

	if (isdigit(ch)) { //int, float
		std::unique_ptr<CToken>token;
		auto [type, number] = getNumber();

		if (type == VariantType::vtInt) {
			int value = stoll(number);
			std::unique_ptr<CVariant> intVariant = std::make_unique<CIntVariant>(value);
			token = std::make_unique<CConstToken>(intVariant.release(), startPos);
		}
		else {
			double value = stod(number);
			std::unique_ptr<CVariant> realVariant = std::make_unique<CRealVariant>(value);
			token = std::make_unique<CConstToken>(realVariant.release(), startPos);
		}

		return token;
	}

	if (isOneCharKeyword(ch)) { //one char keywords
		std::string name;
		name += ch;
		getNextChar();
		return std::make_unique<CKeywordToken>(strToKeywords.at(name), startPos);
	}


	std::string name;
	name += ch;

	char lastCh = ch;
	getNextChar();

	switch (lastCh) {
	case ':':
		if (ch == '=') {
			name += ch;
			getNextChar();
			return std::make_unique<CKeywordToken>(strToKeywords.at(name), startPos);
		}
		else {
			return std::make_unique<CKeywordToken>(strToKeywords.at(name), startPos);
		}
	case '<':
		if (ch == '=' || ch == '>') {
			name += ch;
			getNextChar();
			return std::make_unique<CKeywordToken>(strToKeywords.at(name), startPos);
		}
		else {
			return std::make_unique<CKeywordToken>(strToKeywords.at(name), startPos);
		}
	case '>':
		if (ch == '=') {
			name += ch;
			getNextChar();
			return std::make_unique<CKeywordToken>(strToKeywords.at(name), startPos);
		}
		else {
			return std::make_unique<CKeywordToken>(strToKeywords.at(name), startPos);
		}
	default:
		throw Error(ErrorCodes::UnknownSymbol, startPos, name);
	}

	return nullptr;
}
