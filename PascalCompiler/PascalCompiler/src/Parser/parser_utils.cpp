#include "parser.h"

void CParser::getNextToken() {
	tokenPointer++;
	if (tokenPointer >= tokenBuffer.size()) {
		bool goodToken = false;
		std::unique_ptr<CToken> currentToken;
		while (!goodToken) {
			goodToken = true;
			try {
				currentToken = std::move(lexer->getNextToken());
			}
			catch (CError& e) {
				addError(e);
				goodToken = false;
			}
		}

		tokenBuffer.push_back(std::move(currentToken));
		while (tokenBuffer.size() > BUFF_SIZE)
			tokenBuffer.pop_front();
		tokenPointer = tokenBuffer.size() - 1;
	}
	token = tokenBuffer[tokenPointer];
}

void CParser::rollback() {
	tokenPointer--;
	token = tokenBuffer[tokenPointer];
}

bool CParser::isKeyword() {
	return token->getType() == TokenType::ttKeyword;
}

bool CParser::isIdent() {
	return token->getType() == TokenType::ttIdent;
}

bool CParser::isConst() {
	return token->getType() == TokenType::ttConst;
}

bool CParser::isEOF() {
	return token->getType() == TokenType::ttEOF;
}

KeyWords CParser::getTokenKeyword() {
	if (!isKeyword())
		throw std::exception("Can't convert CToken to CKeywordToken");
	return std::dynamic_pointer_cast<CKeywordToken>(token)->getKeyword();
}

VariantType CParser::getTokenVariantType() {
	if (!isConst())
		throw std::exception("Can't convert CToken to CConstToken");

	std::shared_ptr<CConstToken> constToken = std::dynamic_pointer_cast<CConstToken>(token);
	return constToken->getValue()->getVariantType();
}

bool CParser::keywordEquals(KeyWords keyword) {
	return getTokenKeyword() == keyword;
}

bool CParser::isDeclarationPart() {
	return declarationKeywords.count(getTokenKeyword());
}

bool CParser::isStructuredStatement() {
	return isKeyword() && structuredStatementKeywords.count(getTokenKeyword());
}

bool CParser::isRelationOperator() {
	return isKeyword() && relationOperators.count(getTokenKeyword());
}

bool CParser::isAddingOperator() {
	return isKeyword() && addingOperators.count(getTokenKeyword());
}

bool CParser::isMultiplyingOperator() {
	return isKeyword() && multyplingOperators.count(getTokenKeyword());
}

bool CParser::isUnaryOperator() {
	return isKeyword() && unaryOperators.count(getTokenKeyword());
}

//Error handling
bool CParser::belong(const std::vector<std::shared_ptr<CToken>>& starters) {
	for (auto& starter : starters) {
		if (starter->getType() == token->getType()) {
			if (token->getType() == TokenType::ttConst || token->getType() == TokenType::ttIdent)
				return true;

			auto tokenKeyword = std::dynamic_pointer_cast<CKeywordToken>(token)->getKeyword();
			auto starterKeyword = std::dynamic_pointer_cast<CKeywordToken>(starter)->getKeyword();
			if (tokenKeyword == starterKeyword)
				return true;
		}
	}
	return false;
}

void CParser::skipTo(const std::vector<std::shared_ptr<CToken>>& acceptableTokens) {
	while (!isEOF() && !belong(acceptableTokens)) {
		getNextToken();
	}
}

void CParser::addError(CError err) {
	errorList.push_back(err);
}


//Scope handling

std::shared_ptr<CScope> CParser::initBaseScope() {

	std::shared_ptr<CScope> scope = std::make_shared<CScope>(nullptr);
	std::shared_ptr<CFuncParameters> mainParams = std::make_shared<CFuncParameters>();
	auto mainFunction = gen->initFunction("main", gen->convertToTypePtr(ExprType::eIntType), mainParams);
	auto mainBody = gen->createBlock(mainFunction);
	gen->setInsertionPoint(mainBody);

	scope->addType("integer", ExprType::eIntType);
	scope->addType("real", ExprType::eRealType);
	scope->addType("boolean", ExprType::eBooleanType);

	//This is really bad!
	gen->createVariable("true", "boolean", scope);
	gen->createVariable("false", "boolean", scope);
	gen->createAssignment("true", gen->getTrue(), scope);
	gen->createAssignment("false", gen->getFalse(), scope);

	auto prinfnType = FunctionType::get(gen->convertToTypePtr(ExprType::eBooleanType), true);
	gen->initWrite(scope);

	gen->setInsertionPoint(mainBody);

	return scope;
}
