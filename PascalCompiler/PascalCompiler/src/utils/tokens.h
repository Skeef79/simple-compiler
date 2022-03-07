#pragma once
#include<string>
#include "keywords.h"
#include "utils.h"
#include "variants.h"
#include<memory>

enum class TokenType : char {
	ttIdent,
	ttKeyword,
	ttConst
};

class CToken {
public:
	CToken(TokenType tType, TextPosition pos);

	TokenType getType();
	TextPosition getPosition();

	virtual std::string toString() = 0;
private:
	TokenType tType;
	TextPosition pos;
};


class CIdentToken : public CToken {
public:
	CIdentToken(std::string name, TextPosition pos);
	std::string toString() override;
private:
	std::string name;
};

class CKeywordToken : public CToken {
public:
	CKeywordToken(KeyWords name, TextPosition pos);
	std::string toString() override;
private:
	KeyWords name;
};

class CConstToken : public CToken {
public:
	CConstToken(CVariant* value, TextPosition pos);
	std::string toString() override;
private:
	std::unique_ptr<CVariant> value;
};


void printToken(std::unique_ptr<CToken>& token);