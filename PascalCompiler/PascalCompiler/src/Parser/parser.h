#pragma once
#include "../Lexer/lexer.h"
#include "../Semantics/scope.h"
#include<deque>
#include<vector>
#include "../Codegen/codegen.h"

class CParser {
public:
	CParser(CLexer* lexer);
	void getNextToken();
	void parse();
	std::vector<CError> errorList;
	void printGeneratedCode();
	void compile(std::string filename);

private:
	//store the buffer of BUFF_SIZE elements
	// if needed, move pointer to previous token
	// GetNextToken() should be moving pointer and,  if needed, adding new token
	const int BUFF_SIZE = 4;
	int tokenPointer; //points to current token
	std::deque<std::shared_ptr<CToken>>tokenBuffer;
	std::shared_ptr<CToken> token; //current token
	std::unique_ptr<CLexer> lexer;

	std::shared_ptr<CCodeGenerator> gen;

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

	std::shared_ptr<CScope> initBaseScope();

	bool belong(const std::vector<std::shared_ptr<CToken>>& starters);
	void skipTo(const std::vector<std::shared_ptr<CToken>>& acceptableTokens);

	void addError(CError err);

	KeyWords getTokenKeyword();
	VariantType getTokenVariantType();

	bool isKeyword();
	bool isIdent();
	bool isConst();
	bool isEOF();

	bool keywordEquals(KeyWords keyword);
	bool isDeclarationPart();
	bool isStructuredStatement();
	bool isRelationOperator();
	bool isAddingOperator();
	bool isMultiplyingOperator();
	bool isUnaryOperator();

	std::shared_ptr<CKeywordToken> acceptKeyword(KeyWords expectedKeyword);
	std::shared_ptr<CConstToken> acceptConst(VariantType expectedVariantType);
	std::shared_ptr<CIdentToken> acceptIdent();


	struct FuncHeading {
		std::shared_ptr<CIdentToken>ident;
		std::shared_ptr<CIdentToken>identType;
		std::shared_ptr<CFuncParameters> parameters;
	};

	void program(std::shared_ptr<CScope> scope);
	void block(std::shared_ptr<CScope> scope);
	std::shared_ptr<CIdentToken> identifier(std::shared_ptr<CScope> scope);
	void typeDeclarationPart(std::shared_ptr<CScope> scope);
	void typeDeclaration(std::shared_ptr<CScope> scope);
	std::shared_ptr<CIdentToken> type(std::shared_ptr<CScope> scope);
	void varDeclarationPart(std::shared_ptr<CScope> scope);
	void varDeclaration(std::shared_ptr<CScope> scope);
	void functionDeclarationPart(std::shared_ptr<CScope> scope);
	void functionDeclaration(std::shared_ptr<CScope> scope);
	FuncHeading functionHeading(std::shared_ptr<CScope> scope);
	std::shared_ptr<CFuncParameters> formalParameterSection(std::shared_ptr<CScope> scope);
	std::shared_ptr<CFuncParameters> parameterGroup(std::shared_ptr<CScope> scope, bool byRef);
	void statementPart(std::shared_ptr<CScope> scope);
	void compoundStatement(std::shared_ptr<CScope> scope);
	void statement(std::shared_ptr<CScope> scope);
	void simpleStatement(std::shared_ptr<CScope> scope);
	void assignmentStatement(std::shared_ptr<CScope> scope);
	Value* variable(std::shared_ptr<CScope> scope);
	void procedureStatement(std::shared_ptr<CScope> scope);
	Value* actualParameter(std::shared_ptr<CScope> scope);
	Value* expression(std::shared_ptr<CScope> scope);
	Value* simpleExpression(std::shared_ptr<CScope> scope);
	Value* term(std::shared_ptr<CScope> scope);
	Value* factor(std::shared_ptr<CScope> scope);
	Value* functionDesignator(std::shared_ptr<CScope> scope);
	void structuredStatement(std::shared_ptr<CScope> scope);
	void ifStatement(std::shared_ptr<CScope> scope);
	void whileStatement(std::shared_ptr<CScope> scope);

};

