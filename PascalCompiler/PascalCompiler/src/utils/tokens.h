#pragma once
#include<string>
#include "keywords.h"
#include "utils.h"
#include "variants.h"
#include<memory>

enum class TokenType : char {
	ttIdent,
	ttKeyword,
	ttConst,
	ttEOF
};

const  std::map<TokenType, std::string> tokenTypeToStr = {
	{TokenType::ttIdent, "identifier"},
	{TokenType::ttConst, "const"},
	{TokenType::ttKeyword, "keyword"},
	{TokenType::ttEOF, "eof"}
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
	CIdentToken();
	std::string toString() override;
	std::string getIdent();

private:
	std::string name;
};

class CKeywordToken : public CToken {
public:
	CKeywordToken(KeyWords name, TextPosition pos);
	CKeywordToken(KeyWords name);
	KeyWords getKeyword();

	std::string toString() override;
private:
	KeyWords name;
};

class CConstToken : public CToken {
public:
	CConstToken(CVariant* value, TextPosition pos);
	std::string toString() override;

	const std::unique_ptr<CVariant>& getValue() const;
private:
	std::unique_ptr<CVariant> value;
};

class CEOFToken : public CToken {
public:
	CEOFToken(TextPosition pos);
	std::string toString() override;

};

void printToken(std::unique_ptr<CToken>& token);