#pragma once
#include "../Lexer/lexer.h"
#include<deque>
#include<vector>

class CParser {
public:
	CParser(CLexer* lexer);
	void getNextToken();
	void parse();
	std::vector<Error> errorList;

private:
	//store the buffer of BUFF_SIZE elements
	// if needed, move pointer to previous token
	// GetNextToken() should be moving pointer and,  if needed, adding new token
	const int BUFF_SIZE = 4;
	int tokenPointer; //points to current token
	std::deque<std::shared_ptr<CToken>>tokenBuffer;
	std::shared_ptr<CToken> token; //current token
	std::unique_ptr<CLexer> lexer;

	void rollback(); //moves tokenPointer to previous token

	const std::set<KeyWords> declarationKeywords = {
		KeyWords::typeSy,
		KeyWords::varSy,
		KeyWords::functionSy
	};

	const std::set<KeyWords> structuredStatementKeywords = {
		KeyWords::beginSy,
		KeyWords::ifSy,
		KeyWords::whileSy
	};

	const std::set<KeyWords> relationOperators = {
		KeyWords::lessSy,
		KeyWords::greaterSy,
		KeyWords::lessEqualSy,
		KeyWords::greaterEqualSy,
		KeyWords::equalSy,
		KeyWords::notEqualSy
	};

	const std::set<KeyWords> addingOperators = {
		KeyWords::plusSy,
		KeyWords::minusSy,
		KeyWords::orSy
	};

	const std::set<KeyWords> multyplingOperators = {
		KeyWords::multiplySy,
		KeyWords::divisionSy,
		KeyWords::andSy
	};

	const std::set<KeyWords> unaryOperators = {
		KeyWords::plusSy,
		KeyWords::minusSy,
		KeyWords::notSy
	};

	bool belong(const std::vector<std::shared_ptr<CToken>>& starters);
	void skipTo(const std::vector<std::shared_ptr<CToken>>& acceptableTokens);

	void addError(Error err);

	KeyWords getTokenKeyword();
	VariantType getTokenVariantType();

	bool isKeyword();
	bool isIdent();
	bool isConst();

	bool keywordEquals(KeyWords keyword);
	bool isDeclarationPart();
	bool isStructuredStatement();
	bool isRelationOperator();
	bool isAddingOperator();
	bool isMultiplyingOperator();
	bool isUnaryOperator();

	void acceptKeyword(KeyWords expectedKeyword);
	void acceptConst(VariantType expectedVariantType);
	void acceptIdent();

	void program();
	void block();
	void identifier();
	void typeDeclarationPart();
	void typeDeclaration();
	void type();
	void pointerType();
	void varDeclarationPart();
	void varDeclaration();
	void functionDeclarationPart();
	void functionDeclaration();
	void functionHeading();
	void formalParameterSection();
	void parameterGroup();
	void statementPart();
	void compoundStatement();
	void statement();
	void simpleStatement();
	void assignmentStatement();
	void variable();
	void procedureStatement();
	void actualParameter();
	void expression();
	void simpleExpression();
	void term();
	void factor();
	void functionDesignator();
	void structuredStatement();
	void ifStatement();
	void whileStatement();

};

