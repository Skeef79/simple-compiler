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
	CToken(TokenType tType);
	TokenType getType();
	virtual std::string toString() = 0;
private:
	TokenType tType;
};


class CIdentToken : public CToken {
public:
	CIdentToken(std::string name);
	std::string toString() override;
private:
	std::string name;
};

class CKeywordToken : public CToken {
public:
	CKeywordToken(KeyWords name);
	std::string toString() override;
private:
	KeyWords name;
};

class CConstToken : public CToken {
public:
	CConstToken(CVariant* value);
	std::string toString() override;
private:
	std::unique_ptr<CVariant> value;
};

