#include "parser.h"


CParser::CParser(CLexer* lexer) {
	this->lexer.reset(lexer);
	token = std::move(this->lexer->getNextToken());
	tokenPointer = 0;
}

//accept functions
void CParser::acceptKeyword(KeyWords expectedKeyword) {

	if (!isKeyword())
		throw Error(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

	if (getTokenKeyword() != expectedKeyword)
		throw Error(ErrorCodes::ExpectedToken, token->getPosition(), keywordsToStr.at(expectedKeyword));
	getNextToken();
}

void CParser::acceptConst(VariantType expectedVariantType) {
	if (!isConst())
		throw Error(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

	std::shared_ptr<CConstToken> constToken = std::dynamic_pointer_cast<CConstToken>(token);
	VariantType tokenVariantType = constToken->getValue()->getVariantType();

	if (tokenVariantType != expectedVariantType)
		throw Error(ErrorCodes::IncorrectConstType, token->getPosition(), variantTypeToStr.at(tokenVariantType));
	getNextToken();
}

void CParser::acceptIdent() {
	if (!isIdent())
		throw Error(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));
	getNextToken();
}


//TODO:
// nullptr check
// ....
// 


//main bnf part
void CParser::parse() {
	program();
}

void CParser::program() {
	const std::vector<std::shared_ptr<CToken>> acceptableTokens = {
		std::make_shared<CKeywordToken>(KeyWords::varSy),
		std::make_shared<CKeywordToken>(KeyWords::typeSy),
		std::make_shared<CKeywordToken>(KeyWords::functionSy),
		std::make_shared<CKeywordToken>(KeyWords::beginSy)
	};

	try {
		if (token->getType() == TokenType::ttKeyword && keywordEquals(KeyWords::programSy)) {
			acceptKeyword(KeyWords::programSy);
			acceptIdent();
			acceptKeyword(KeyWords::semicolonSy);
		}
	}
	catch (Error& e) {
		addError(e);
		skipTo(acceptableTokens);
	}

	block();
	try {
		acceptKeyword(KeyWords::dotSy);
	}
	catch (Error& e) {
		addError(e);
	}
}

void CParser::block() {
	while (isKeyword() && isDeclarationPart()) {
		auto tokenKeyword = getTokenKeyword();
		switch (tokenKeyword) {
		case KeyWords::typeSy:
			typeDeclarationPart();
			break;
		case KeyWords::varSy:
			varDeclarationPart();
			break;
		case KeyWords::functionSy:
			functionDeclarationPart();
			break;
		}
	}
	statementPart();
}

void CParser::identifier() {
	acceptIdent();
}

void CParser::typeDeclarationPart() {
	const std::vector<std::shared_ptr<CToken>> acceptableTokens = {
		std::make_shared<CKeywordToken>(KeyWords::varSy),
		std::make_shared<CKeywordToken>(KeyWords::typeSy),
		std::make_shared<CKeywordToken>(KeyWords::functionSy),
		std::make_shared<CKeywordToken>(KeyWords::beginSy)
	};
	try {
		acceptKeyword(KeyWords::typeSy);
		while (isIdent()) {
			typeDeclaration();
			acceptKeyword(KeyWords::semicolonSy);
		}
	}
	catch (Error& e) {
		addError(e);
		skipTo(acceptableTokens);
	}
}

void CParser::typeDeclaration() {
	acceptIdent();
	acceptKeyword(KeyWords::equalSy);
	type();
}

void CParser::type() {
	if (isKeyword())
		pointerType();
	else
		acceptIdent();
}

void CParser::pointerType() {
	acceptKeyword(KeyWords::pointerSy);
	acceptIdent();
}

void CParser::varDeclarationPart() {
	const std::vector<std::shared_ptr<CToken>> acceptableTokens = {
		std::make_shared<CKeywordToken>(KeyWords::varSy),
		std::make_shared<CKeywordToken>(KeyWords::typeSy),
		std::make_shared<CKeywordToken>(KeyWords::functionSy),
		std::make_shared<CKeywordToken>(KeyWords::beginSy)
	};
	try {
		acceptKeyword(KeyWords::varSy);
		while (isIdent()) {
			varDeclaration();
			acceptKeyword(KeyWords::semicolonSy);
		}
	}
	catch (Error& e) {
		addError(e);
		skipTo(acceptableTokens);
	}

}

void CParser::varDeclaration() {
	bool isVarDeclaration = true;
	while (isVarDeclaration) {
		acceptIdent();
		if (!isKeyword())
			throw Error(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

		if (keywordEquals(KeyWords::commaSy)) {
			acceptKeyword(KeyWords::commaSy);
		}
		else {
			acceptKeyword(KeyWords::colonSy);
			isVarDeclaration = false;
		}
	}
	type();
}

void CParser::functionDeclarationPart() {
	const std::vector<std::shared_ptr<CToken>> acceptableTokens = {
		std::make_shared<CKeywordToken>(KeyWords::varSy),
		std::make_shared<CKeywordToken>(KeyWords::typeSy),
		std::make_shared<CKeywordToken>(KeyWords::functionSy),
		std::make_shared<CKeywordToken>(KeyWords::beginSy)
	};

	try {
		while (isKeyword() && keywordEquals(KeyWords::functionSy)) {
			functionDeclaration();
			acceptKeyword(KeyWords::semicolonSy);
		}
	}
	catch (Error& e) {
		addError(e);
		skipTo(acceptableTokens);
	}
}

void CParser::functionDeclaration() {
	functionHeading();
	block();
}

void CParser::functionHeading() {
	acceptKeyword(KeyWords::functionSy);
	acceptIdent();

	if (!isKeyword())
		throw Error(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

	if (keywordEquals(KeyWords::colonSy)) {
		acceptKeyword(KeyWords::colonSy);
		type();
		acceptKeyword(KeyWords::semicolonSy);
	}
	else {
		acceptKeyword(KeyWords::leftParSy);

		if (isIdent() || isKeyword() && keywordEquals(KeyWords::varSy)) {
			formalParameterSection();
		}

		while (isKeyword() && keywordEquals(KeyWords::semicolonSy)) {
			acceptKeyword(KeyWords::semicolonSy);
			formalParameterSection();
		}

		acceptKeyword(KeyWords::rightParSy);
		acceptKeyword(KeyWords::colonSy);
		type();
		acceptKeyword(KeyWords::semicolonSy);
	}

}
void CParser::formalParameterSection() {
	if (isKeyword()) {
		acceptKeyword(KeyWords::varSy);
	}
	parameterGroup();
}

void CParser::parameterGroup() {
	varDeclaration();
}

void CParser::statementPart() {
	compoundStatement();
}

void CParser::compoundStatement() {

	acceptKeyword(KeyWords::beginSy);

	statement();
	while (isKeyword() && keywordEquals(KeyWords::semicolonSy)) {
		acceptKeyword(KeyWords::semicolonSy);
		statement();
	}

	acceptKeyword(KeyWords::endSy);
}

void CParser::statement() {
	//simple statement or structured statement
	const std::vector<std::shared_ptr<CToken>> acceptableTokens = {
		std::make_shared<CIdentToken>(),
		std::make_shared<CKeywordToken>(KeyWords::beginSy),
		std::make_shared<CKeywordToken>(KeyWords::ifSy),
		std::make_shared<CKeywordToken>(KeyWords::whileSy)
	};

	try {
		if (isIdent())
			simpleStatement();
		else {
			if (isStructuredStatement())
				structuredStatement();
		}
	}
	catch (Error& e) {
		addError(e);
		skipTo(acceptableTokens);
	}
}

void CParser::simpleStatement() {
	if (!isIdent())
		throw Error(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

	acceptIdent();
	if (!isKeyword())
		throw Error(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

	if (keywordEquals(KeyWords::assignSy) || keywordEquals(KeyWords::pointerSy)) {
		rollback();
		assignmentStatement();
	}
	else {
		rollback();
		procedureStatement();
	}
}


void CParser::assignmentStatement() {
	variable();
	acceptKeyword(KeyWords::assignSy);
	expression();
}

void CParser::variable() {
	acceptIdent();
	while (isKeyword() && keywordEquals(KeyWords::pointerSy)) {
		acceptKeyword(KeyWords::pointerSy);
	}
}

void CParser::expression() {
	simpleExpression();
	if (isKeyword() && isRelationOperator()) {
		acceptKeyword(getTokenKeyword());
		simpleExpression();
	}
}

void CParser::simpleExpression() {
	term();
	while (isKeyword() && isAddingOperator()) {
		acceptKeyword(getTokenKeyword());
		term();
	}
}


void CParser::term() {
	factor();
	while (isKeyword() && isMultiplyingOperator()) {
		acceptKeyword(getTokenKeyword());
		factor();
	}
}

void CParser::factor() {
	if (isUnaryOperator()) {
		acceptKeyword(getTokenKeyword());
		factor();
		return;
	}

	if (isConst()) {
		acceptConst(getTokenVariantType());
		return;
	}

	if (isIdent()) {
		acceptIdent();
		if (isKeyword() && keywordEquals(KeyWords::pointerSy)) {
			rollback();
			variable();
		}
		else {
			rollback();
			functionDesignator();
		}
		return;
	}

	acceptKeyword(KeyWords::leftParSy);
	expression();
	acceptKeyword(KeyWords::rightParSy);
}

void CParser::procedureStatement() {
	acceptIdent();

	if (isKeyword() && keywordEquals(KeyWords::leftParSy)) {
		acceptKeyword(KeyWords::leftParSy);
		if (!(isKeyword() && keywordEquals(KeyWords::rightParSy))) {
			actualParameter();
			while (isKeyword() && keywordEquals(KeyWords::commaSy)) {
				acceptKeyword(KeyWords::commaSy);
				actualParameter();
			}
		}
		acceptKeyword(KeyWords::rightParSy);
	}
}


void CParser::functionDesignator() {
	acceptIdent();
	if (isKeyword() && keywordEquals(KeyWords::leftParSy)) {
		acceptKeyword(KeyWords::leftParSy);
		actualParameter();
		while (isKeyword() && keywordEquals(KeyWords::commaSy)) {
			acceptKeyword(KeyWords::commaSy);
			actualParameter();
		}
		acceptKeyword(KeyWords::rightParSy);
	}
}

void CParser::actualParameter() {
	if (isKeyword() && keywordEquals(KeyWords::rightParSy))
		return;
	expression();
}

void CParser::structuredStatement() {
	if (!isKeyword())
		throw Error(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

	if (keywordEquals(KeyWords::beginSy)) {
		compoundStatement();
		return;
	}
	if (keywordEquals(KeyWords::ifSy)) {
		ifStatement();
		return;
	}

	if (keywordEquals(KeyWords::whileSy)) {
		whileStatement();
		return;
	}

	throw Error(ErrorCodes::UnexpectedKeyword, token->getPosition(), keywordsToStr.at(getTokenKeyword()));
}

void CParser::ifStatement() {
	acceptKeyword(KeyWords::ifSy);
	expression();
	acceptKeyword(KeyWords::thenSy);
	statement();
	if (isKeyword() && keywordEquals(KeyWords::elseSy)) {
		acceptKeyword(KeyWords::elseSy);
		statement();
	}
}

void CParser::whileStatement() {
	acceptKeyword(KeyWords::whileSy);
	expression();
	acceptKeyword(KeyWords::doSy);
	statement();
}


