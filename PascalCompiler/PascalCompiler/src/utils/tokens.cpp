#include "tokens.h"
#include<string>
#include<iostream>
#include<iomanip>

CToken::CToken(TokenType tType, TextPosition pos)
	: tType(tType), pos(pos) {}

TokenType CToken::getType() {
	return tType;
}

TextPosition CToken::getPosition() {
	return pos;
}

CIdentToken::CIdentToken(std::string name, TextPosition pos)
	: CToken(TokenType::ttIdent, pos), name(name) {}

CIdentToken::CIdentToken()
	: CToken(TokenType::ttIdent, TextPosition(-1, -1)) {}

std::string CIdentToken::toString() {
	return name + " [ ttIdent ]";
}

CKeywordToken::CKeywordToken(KeyWords name, TextPosition pos)
	: CToken(TokenType::ttKeyword, pos), name(name) {}

CKeywordToken::CKeywordToken(KeyWords name)
	: CToken(TokenType::ttKeyword, TextPosition(-1, -1)), name(name) {}


std::string CKeywordToken::toString() {
	return keywordsToStr.at(name) + " [ ttKeyword ]";
}

CConstToken::CConstToken(CVariant* value, TextPosition pos)
	: CToken(TokenType::ttConst, pos) {
	this->value.reset(value);
}

std::string CConstToken::toString() {
	return value->toString() + " ( " + variantTypeToStr.at(value->getVariantType()) + " )" + " [ ttConst ]";
}

void printToken(std::unique_ptr<CToken>& token) {

	auto pos = token->getPosition();
	std::string leftInfo = std::to_string(pos.lineIndex + 1) + ":" + std::to_string(pos.charIndex + 1) + ": ";
	std::cout << std::setw(10) << std::left << leftInfo;
	std::cout << token->toString() << std::endl;
}


std::string CIdentToken::getIdent() {
	return name;
}

KeyWords CKeywordToken::getKeyword() {
	return name;
}

const std::unique_ptr<CVariant>& CConstToken::getValue() const {
	return value;
}