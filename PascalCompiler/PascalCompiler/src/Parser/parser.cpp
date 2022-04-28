#include "parser.h"


CParser::CParser(CLexer* lexer) {
	this->lexer.reset(lexer);
	bool goodToken = false;
	while (!goodToken) {
		goodToken = true;
		try {
			token = std::move(this->lexer->getNextToken());
		}
		catch (Error& e) {
			addError(e);
			goodToken = false;
		}
	}
	tokenPointer = 0;
}

//accept functions
std::shared_ptr<CKeywordToken> CParser::acceptKeyword(KeyWords expectedKeyword) {

	if (!isKeyword())
		throw Error(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

	if (getTokenKeyword() != expectedKeyword)
		throw Error(ErrorCodes::ExpectedToken, token->getPosition(), keywordsToStr.at(expectedKeyword));
	auto keywordToken = std::dynamic_pointer_cast<CKeywordToken>(token);
	getNextToken();
	return keywordToken;
}

std::shared_ptr<CConstToken> CParser::acceptConst(VariantType expectedVariantType) {
	if (!isConst())
		throw Error(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

	std::shared_ptr<CConstToken> constToken = std::dynamic_pointer_cast<CConstToken>(token);
	VariantType tokenVariantType = constToken->getValue()->getVariantType();

	if (tokenVariantType != expectedVariantType)
		throw Error(ErrorCodes::IncorrectConstType, token->getPosition(), variantTypeToStr.at(tokenVariantType));
	getNextToken();
	return constToken;
}

std::shared_ptr<CIdentToken> CParser::acceptIdent() {
	if (!isIdent())
		throw Error(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));
	auto identToken = std::dynamic_pointer_cast<CIdentToken>(token);
	getNextToken();
	return identToken;
}

//main bnf part
void CParser::parse() {
	auto baseScope = initBaseScope();
	auto scope = std::make_shared<CScope>(baseScope);
	program(scope);
}

void CParser::program(std::shared_ptr<CScope> scope) {
	const std::vector<std::shared_ptr<CToken>> acceptableTokens = {
		std::make_shared<CKeywordToken>(KeyWords::varSy),
		std::make_shared<CKeywordToken>(KeyWords::typeSy),
		std::make_shared<CKeywordToken>(KeyWords::functionSy),
		std::make_shared<CKeywordToken>(KeyWords::beginSy)
	};

	try {
		if (token->getType() == TokenType::ttKeyword && keywordEquals(KeyWords::programSy)) {
			acceptKeyword(KeyWords::programSy);
			auto ident = identifier(scope);
			scope->addIdent(ident->getIdent(), "string");
			acceptKeyword(KeyWords::semicolonSy);
		}
	}
	catch (Error& e) {
		addError(e);
		skipTo(acceptableTokens);
	}

	const std::vector<std::shared_ptr<CToken>> acceptableTokensBlock = {
		std::make_shared<CKeywordToken>(KeyWords::dotSy)
	};

	try {
		auto childScope = std::make_shared<CScope>(scope);
		block(childScope);
		acceptKeyword(KeyWords::dotSy);
	}
	catch (Error& e) {
		addError(e);
	}

}

void CParser::block(std::shared_ptr<CScope> scope) {
	while (isKeyword() && isDeclarationPart()) {
		auto tokenKeyword = getTokenKeyword();
		switch (tokenKeyword) {
		case KeyWords::typeSy:
			typeDeclarationPart(scope);
			break;
		case KeyWords::varSy:
			varDeclarationPart(scope);
			break;
		case KeyWords::functionSy:
			functionDeclarationPart(scope);
			break;
		}
	}
	statementPart(scope);
}

std::shared_ptr<CIdentToken> CParser::identifier(std::shared_ptr<CScope> scope) {
	return acceptIdent();
}

void CParser::typeDeclarationPart(std::shared_ptr<CScope> scope) {
	const std::vector<std::shared_ptr<CToken>> acceptableTokens = {
		std::make_shared<CKeywordToken>(KeyWords::varSy),
		std::make_shared<CKeywordToken>(KeyWords::typeSy),
		std::make_shared<CKeywordToken>(KeyWords::functionSy),
		std::make_shared<CKeywordToken>(KeyWords::beginSy)
	};
	try {
		acceptKeyword(KeyWords::typeSy);
		while (isIdent()) {
			typeDeclaration(scope);
			acceptKeyword(KeyWords::semicolonSy);
		}
	}
	catch (Error& e) {
		addError(e);
		skipTo(acceptableTokens);
	}
}

void CParser::typeDeclaration(std::shared_ptr<CScope> scope) {
	auto ident = identifier(scope);
	acceptKeyword(KeyWords::equalSy);
	auto identType = type(scope);

	if (scope->identDefinedInScope(ident->getIdent())) {
		addError(Error(ErrorCodes::IdentifierAlreadyDefined, ident->getPosition(), ident->getIdent()));
	}
	else {
		if (!scope->typeDefined(identType->getIdent())) {
			addError(Error(ErrorCodes::IdentifierNotDefined, identType->getPosition(), identType->getIdent()));
		}
		else {
			auto tType = scope->getTypeforType(identType->getIdent());
			scope->addType(ident->getIdent(), tType);
		}
	}
}

std::shared_ptr<CIdentToken> CParser::type(std::shared_ptr<CScope> scope) {
	if (isEOF())
		throw Error(ErrorCodes::UnexpectedToken, token->getPosition(), "EOF");

	if (isKeyword())
		return pointerType(scope);
	else
		return identifier(scope);
}

std::shared_ptr<CIdentToken> CParser::pointerType(std::shared_ptr<CScope> scope) {
	acceptKeyword(KeyWords::pointerSy);
	return acceptIdent();
}

void CParser::varDeclarationPart(std::shared_ptr<CScope> scope) {
	const std::vector<std::shared_ptr<CToken>> acceptableTokens = {
		std::make_shared<CKeywordToken>(KeyWords::varSy),
		std::make_shared<CKeywordToken>(KeyWords::typeSy),
		std::make_shared<CKeywordToken>(KeyWords::functionSy),
		std::make_shared<CKeywordToken>(KeyWords::beginSy)
	};
	try {
		acceptKeyword(KeyWords::varSy);
		while (isIdent()) {
			varDeclaration(scope);
			acceptKeyword(KeyWords::semicolonSy);
		}
	}
	catch (Error& e) {
		addError(e);
		skipTo(acceptableTokens);
	}

}

void CParser::varDeclaration(std::shared_ptr<CScope> scope) {
	bool isVarDeclaration = true;
	std::vector<std::shared_ptr<CIdentToken>> varIdents;

	while (isVarDeclaration) {
		varIdents.push_back(identifier(scope));

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


	auto identType = type(scope);
	if (!scope->typeDefined(identType->getIdent())) {
		addError(Error(ErrorCodes::IdentifierNotDefined, identType->getPosition(), identType->getIdent()));
	}
	else {
		for (auto ident : varIdents) {
			if (scope->identDefinedInScope(ident->getIdent())) {
				addError(Error(ErrorCodes::IdentifierAlreadyDefined, ident->getPosition(), ident->getIdent()));
			}
			else {
				scope->addIdent(ident->getIdent(), identType->getIdent());
			}
		}
	}
}

void CParser::functionDeclarationPart(std::shared_ptr<CScope> scope) {
	const std::vector<std::shared_ptr<CToken>> acceptableTokens = {
		std::make_shared<CKeywordToken>(KeyWords::varSy),
		std::make_shared<CKeywordToken>(KeyWords::typeSy),
		std::make_shared<CKeywordToken>(KeyWords::functionSy),
		std::make_shared<CKeywordToken>(KeyWords::beginSy)
	};

	try {
		while (isKeyword() && keywordEquals(KeyWords::functionSy)) {
			functionDeclaration(scope);
			acceptKeyword(KeyWords::semicolonSy);
		}
	}
	catch (Error& e) {
		addError(e);
		skipTo(acceptableTokens);
	}
}

void CParser::functionDeclaration(std::shared_ptr<CScope> scope) {
	auto functionScope = std::make_shared<CScope>(scope);
	auto [functionIdent, functionType, functionParameters] = functionHeading(functionScope);
	if (scope->identDefinedInScope(functionIdent->getIdent())) {
		addError(Error(ErrorCodes::IdentifierAlreadyDefined, functionIdent->getPosition(), functionIdent->getIdent()));
	}
	if (!scope->typeDefined(functionType->getIdent())) {
		addError(Error(ErrorCodes::IdentifierNotDefined, functionType->getPosition(), functionType->getIdent()));
	}
	scope->addFunction(functionIdent->getIdent(), functionType->getIdent(), functionParameters);

	block(functionScope);
}

CParser::FuncHeading CParser::functionHeading(std::shared_ptr<CScope> scope) {
	acceptKeyword(KeyWords::functionSy);
	auto functionIdent = identifier(scope);
	auto functionParameters = std::make_shared<CFuncParameters>();

	if (!isKeyword())
		throw Error(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

	if (keywordEquals(KeyWords::colonSy)) {
		acceptKeyword(KeyWords::colonSy);
		auto functionType = type(scope);
		if (!scope->typeDefined(functionType->getIdent())) {
			addError(Error(ErrorCodes::IdentifierNotDefined, functionType->getPosition(), functionType->getIdent()));
		}

		acceptKeyword(KeyWords::semicolonSy);
		scope->addFunction(functionIdent->getIdent(), functionType->getIdent(), functionParameters);
		scope->addIdent(functionIdent->getIdent(), functionType->getIdent());
		return { functionIdent, functionType, functionParameters };
	}
	else {
		acceptKeyword(KeyWords::leftParSy);

		if (isIdent() || isKeyword() && keywordEquals(KeyWords::varSy)) {
			functionParameters->addParameters(formalParameterSection(scope));
		}

		while (isKeyword() && keywordEquals(KeyWords::semicolonSy)) {
			acceptKeyword(KeyWords::semicolonSy);
			functionParameters->addParameters(formalParameterSection(scope));
		}

		acceptKeyword(KeyWords::rightParSy);
		acceptKeyword(KeyWords::colonSy);
		auto functionType = type(scope);
		if (!scope->typeDefined(functionType->getIdent())) {
			addError(Error(ErrorCodes::IdentifierNotDefined, functionType->getPosition(), functionType->getIdent()));
		}
		acceptKeyword(KeyWords::semicolonSy);

		scope->addFunction(functionIdent->getIdent(), functionType->getIdent(), functionParameters);
		scope->addIdent(functionIdent->getIdent(), functionType->getIdent());
		return { functionIdent, functionType, functionParameters };
	}

}
std::shared_ptr<CFuncParameters> CParser::formalParameterSection(std::shared_ptr<CScope> scope) {
	bool byRef = false;
	if (isKeyword()) {
		acceptKeyword(KeyWords::varSy);
		byRef = true;
	}

	return parameterGroup(scope, byRef);
}

std::shared_ptr<CFuncParameters> CParser::parameterGroup(std::shared_ptr<CScope> scope, bool byRef = false) {
	bool isVarDeclaration = true;
	auto parameters = std::make_shared<CFuncParameters>();
	std::vector<std::shared_ptr<CIdentToken>> varIdents;

	while (isVarDeclaration) {
		auto ident = identifier(scope);
		varIdents.push_back(ident);
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

	auto identType = type(scope);
	ExprType pType;
	if (!scope->typeDefined(identType->getIdent())) {
		addError(Error(ErrorCodes::IdentifierNotDefined, identType->getPosition(), identType->getIdent()));
		pType = ExprType::eErrorType;
	}
	else {
		pType = scope->getTypeforType(identType->getIdent());
	}

	for (auto ident : varIdents) {
		if (scope->identDefinedInScope(ident->getIdent())) {
			addError(Error(ErrorCodes::IdentifierAlreadyDefined, ident->getPosition(), ident->getIdent()));
		}
		else {
			scope->addIdent(ident->getIdent(), identType->getIdent());
		}
		parameters->addParameter(std::make_shared<CParameter>(pType, byRef));
	}

	return parameters;
}

void CParser::statementPart(std::shared_ptr<CScope> scope) {
	compoundStatement(scope);
}

void CParser::compoundStatement(std::shared_ptr<CScope> scope) {

	acceptKeyword(KeyWords::beginSy);

	statement(scope);
	while (isKeyword() && keywordEquals(KeyWords::semicolonSy)) {
		acceptKeyword(KeyWords::semicolonSy);
		statement(scope);
	}

	acceptKeyword(KeyWords::endSy);
}

void CParser::statement(std::shared_ptr<CScope> scope) {
	//simple statement or structured statement
	const std::vector<std::shared_ptr<CToken>> acceptableTokens = {
		std::make_shared<CIdentToken>(),
		std::make_shared<CKeywordToken>(KeyWords::beginSy),
		std::make_shared<CKeywordToken>(KeyWords::ifSy),
		std::make_shared<CKeywordToken>(KeyWords::whileSy)
	};

	try {
		if (isIdent())
			simpleStatement(scope);
		else {
			if (isStructuredStatement())
				structuredStatement(scope);
		}
	}
	catch (Error& e) {
		addError(e);
		skipTo(acceptableTokens);
	}
}

void CParser::simpleStatement(std::shared_ptr<CScope> scope) {
	if (!isIdent())
		throw Error(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

	acceptIdent();
	if (!isKeyword())
		throw Error(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

	if (keywordEquals(KeyWords::assignSy) || keywordEquals(KeyWords::pointerSy)) {
		rollback();
		assignmentStatement(scope);
	}
	else {
		rollback();
		procedureStatement(scope);
	}
}


void CParser::assignmentStatement(std::shared_ptr<CScope> scope) {
	auto pos = token->getPosition();
	ExprType varType = variable(scope);

	acceptKeyword(KeyWords::assignSy);
	ExprType exprType = expression(scope);
	if (!isDerived(varType, exprType)) {
		addError(Error(ErrorCodes::TypeMismatch, pos, exprTypeToStr.at(exprType) + " to " + exprTypeToStr.at(varType)));
	}

}

ExprType CParser::variable(std::shared_ptr<CScope> scope) {
	auto ident = identifier(scope);
	if (!scope->identDefinedGlobal(ident->getIdent())) {
		addError(Error(ErrorCodes::IdentifierNotDefined, ident->getPosition(), ident->getIdent()));
		return ExprType::eErrorType;
	}
	while (isKeyword() && keywordEquals(KeyWords::pointerSy)) {
		acceptKeyword(KeyWords::pointerSy);
	}
	return scope->getIdentType(ident->getIdent());
}

ExprType CParser::expression(std::shared_ptr<CScope> scope) {
	auto pos = token->getPosition();
	ExprType leftExpr = simpleExpression(scope);

	if (isKeyword() && isRelationOperator()) {
		acceptKeyword(getTokenKeyword());
		ExprType rightExpr = simpleExpression(scope);
		if (!isDerived(leftExpr, rightExpr)) {
			addError(Error(ErrorCodes::TypeMismatch, pos, exprTypeToStr.at(leftExpr) + " to " + exprTypeToStr.at(rightExpr)));
			leftExpr = ExprType::eErrorType;
		}

		if (leftExpr != ExprType::eErrorType)
			leftExpr = rightExpr;
	}

	return leftExpr;
}

ExprType CParser::simpleExpression(std::shared_ptr<CScope> scope) {
	auto pos = token->getPosition();
	ExprType leftTerm = term(scope);
	
	while (isKeyword() && isAddingOperator()) {
		//TODO: 
		// operations for types

		auto operation = getTokenKeyword();
		acceptKeyword(getTokenKeyword());
		ExprType rightTerm = term(scope);
		if (!isDerived(leftTerm, rightTerm)) {
			addError(Error(ErrorCodes::TypeMismatch, pos, exprTypeToStr.at(leftTerm) + " to " + exprTypeToStr.at(rightTerm)));
			leftTerm = ExprType::eErrorType;
		}
		else {
			//we can sum everything 
			if (operation == KeyWords::plusSy) {
				leftTerm = rightTerm;
			}
			//minus can't applied to string
			if (operation == KeyWords::minusSy) {
				if (leftTerm == ExprType::eStringType) {
					addError(Error(ErrorCodes::TypeMismatch, pos, exprTypeToStr.at(leftTerm) + " to " + exprTypeToStr.at(rightTerm)));
					leftTerm = ExprType::eErrorType;
				}
				else {
					leftTerm = rightTerm;
				}
			}
			//should be bool or convertable to bool
			if (operation == KeyWords::orSy) {
				if (leftTerm == ExprType::eStringType) {
					addError(Error(ErrorCodes::TypeMismatch, pos, exprTypeToStr.at(leftTerm) + " to " + exprTypeToStr.at(rightTerm)));
					leftTerm = ExprType::eErrorType;
				}
				else {
					leftTerm = rightTerm;
				}
			}
		}

	}
	return leftTerm;
}


ExprType CParser::term(std::shared_ptr<CScope> scope) {
	auto pos = token->getPosition();
	ExprType leftFactor = factor(scope);
	while (isKeyword() && isMultiplyingOperator()) {
		auto operation = getTokenKeyword();
		acceptKeyword(getTokenKeyword());
		ExprType rightFactor = factor(scope);
		if (!isDerived(leftFactor, rightFactor)) {
			addError(Error(ErrorCodes::TypeMismatch, pos, exprTypeToStr.at(leftFactor) + " to " + exprTypeToStr.at(rightFactor)));
			leftFactor = ExprType::eErrorType;
		}
		else {
			//can't 'multiply' strings
			if (operation == KeyWords::multiplySy) {
				if (leftFactor == ExprType::eStringType) {
					addError(Error(ErrorCodes::TypeMismatch, pos, exprTypeToStr.at(leftFactor) + " to " + exprTypeToStr.at(rightFactor)));
					leftFactor = ExprType::eErrorType;
				}
				else {
					leftFactor = rightFactor;
				}
			}
			//can't 'divide' strings
			if (operation == KeyWords::divisionSy) {
				if (leftFactor == ExprType::eStringType) {
					addError(Error(ErrorCodes::TypeMismatch, pos, exprTypeToStr.at(leftFactor) + " to " + exprTypeToStr.at(rightFactor)));
					leftFactor = ExprType::eErrorType;
				}
				else {
					leftFactor = rightFactor;
				}
			}
			//can't 'and' strings
			if (operation == KeyWords::andSy) {
				if (leftFactor == ExprType::eStringType) {
					addError(Error(ErrorCodes::TypeMismatch, pos, exprTypeToStr.at(leftFactor) + " to " + exprTypeToStr.at(rightFactor)));
					leftFactor = ExprType::eErrorType;
				}
				else {
					leftFactor = rightFactor;
				}
			}
		}
	}

	return leftFactor;
}

ExprType CParser::factor(std::shared_ptr<CScope> scope) { //bool isUnary?
	if (isUnaryOperator()) {
		//can't be string
		acceptKeyword(getTokenKeyword());
		return factor(scope);
	}

	if (isConst()) {
		auto constToken = acceptConst(getTokenVariantType());
		auto variantType = constToken->getValue()->getVariantType();
		return variantTypeToExprType.at(variantType);
	}

	if (isIdent()) {
		auto ident = identifier(scope);
		if (scope->identDefinedGlobal(ident->getIdent())) {
			if (scope->isFunction(ident->getIdent())) {
				rollback();
				return functionDesignator(scope);
			}
			else {
				rollback();
				return variable(scope);
			}
		}
		else {
			addError(Error(ErrorCodes::IdentifierNotDefined, ident->getPosition(), ident->getIdent()));
			return ExprType::eErrorType;
		}
	}

	acceptKeyword(KeyWords::leftParSy);
	ExprType exprType = expression(scope);
	acceptKeyword(KeyWords::rightParSy);
	return exprType;
}

void CParser::procedureStatement(std::shared_ptr<CScope> scope) {
	auto pos = token->getPosition();
	auto functionIdent = acceptIdent();
	std::vector<ExprType> parameters;

	if (isKeyword() && keywordEquals(KeyWords::leftParSy)) {
		acceptKeyword(KeyWords::leftParSy);
		if (isKeyword() && keywordEquals(KeyWords::rightParSy)) {
			acceptKeyword(KeyWords::rightParSy);
		}
		else {
			parameters.push_back(actualParameter(scope));
			while (isKeyword() && keywordEquals(KeyWords::commaSy)) {
				acceptKeyword(KeyWords::commaSy);
				parameters.push_back(actualParameter(scope));
			}
			acceptKeyword(KeyWords::rightParSy);
		}
	}

	if (!scope->identDefinedGlobal(functionIdent->getIdent())) {
		addError(Error(ErrorCodes::IdentifierNotDefined, functionIdent->getPosition(), functionIdent->getIdent()));
		return;
	}

	if (!compareParams(parameters, scope->getFunctionParameters(functionIdent->getIdent()))) {
		addError(Error(ErrorCodes::IncorrectParameters, pos, ""));
	}

}


ExprType CParser::functionDesignator(std::shared_ptr<CScope> scope) {
	auto pos = token->getPosition();
	auto functionIdent = acceptIdent();
	std::vector<ExprType> parameters;
	if (isKeyword() && keywordEquals(KeyWords::leftParSy)) {
		acceptKeyword(KeyWords::leftParSy);
		if (isKeyword() && keywordEquals(KeyWords::rightParSy)) {
			acceptKeyword(KeyWords::rightParSy);
		}
		else {
			parameters.push_back(actualParameter(scope));
			while (isKeyword() && keywordEquals(KeyWords::commaSy)) {
				acceptKeyword(KeyWords::commaSy);
				parameters.push_back(actualParameter(scope));
			}
			acceptKeyword(KeyWords::rightParSy);
		}
	}

	if (!scope->identDefinedGlobal(functionIdent->getIdent())) {
		addError(Error(ErrorCodes::IdentifierNotDefined, functionIdent->getPosition(), functionIdent->getIdent()));
		return ExprType::eErrorType;
	}

	if (!compareParams(parameters, scope->getFunctionParameters(functionIdent->getIdent()))) {
		addError(Error(ErrorCodes::IncorrectParameters,pos, ""));
	}
	return scope->getIdentType(functionIdent->getIdent());
}

ExprType CParser::actualParameter(std::shared_ptr<CScope> scope) {
	return expression(scope);
}

void CParser::structuredStatement(std::shared_ptr<CScope> scope) {
	if (!isKeyword())
		throw Error(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

	if (keywordEquals(KeyWords::beginSy)) {
		compoundStatement(scope);
		return;
	}
	if (keywordEquals(KeyWords::ifSy)) {
		ifStatement(scope);
		return;
	}

	if (keywordEquals(KeyWords::whileSy)) {
		whileStatement(scope);
		return;
	}

	throw Error(ErrorCodes::UnexpectedKeyword, token->getPosition(), keywordsToStr.at(getTokenKeyword()));
}

void CParser::ifStatement(std::shared_ptr<CScope> scope) {
	acceptKeyword(KeyWords::ifSy);
	auto pos = token->getPosition();
	ExprType exprType = expression(scope);
	if (!isDerived(exprType, ExprType::eBooleanType)) {
		addError(Error(ErrorCodes::IncorrectExprType, pos, exprTypeToStr.at(exprType)));
	}

	acceptKeyword(KeyWords::thenSy);
	statement(scope);
	if (isKeyword() && keywordEquals(KeyWords::elseSy)) {
		acceptKeyword(KeyWords::elseSy);
		statement(scope);
	}
}

void CParser::whileStatement(std::shared_ptr<CScope> scope) {
	acceptKeyword(KeyWords::whileSy);
	auto pos = token->getPosition();
	ExprType exprType = expression(scope);
	if (!isDerived(exprType,ExprType::eBooleanType)) {
		addError(Error(ErrorCodes::IncorrectExprType, pos, exprTypeToStr.at(exprType)));
	}
	acceptKeyword(KeyWords::doSy);
	statement(scope);
}


