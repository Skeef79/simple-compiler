#include "tokens.h"
#include "variants.h"
#include<string>

CToken::CToken(TokenType tType) : tType(tType) {}

TokenType CToken::getType() {
	return tType;
}

CIdentToken::CIdentToken(std::string name)
	: CToken(TokenType::ttIdent), name(name) {}


std::string CIdentToken::toString() {
	return name + " [ ttIdent ]";
}

CKeywordToken::CKeywordToken(KeyWords name)
	: CToken(TokenType::ttKeyword), name(name) {}

std::string CKeywordToken::toString() {
	return keywordsToStr.at(name) + " [ ttKeyword ]";
}

CConstToken::CConstToken(CVariant* value)
	: CToken(TokenType::ttConst) {
	this->value.reset(value);
}

std::string CConstToken::toString() {
	return value->toString() + " ( " + variantTypeToStr.at(value->getVariantType()) + " )" + " [ ttConst ]";
}