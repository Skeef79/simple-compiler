#include "parser.h"


CParser::CParser(CLexer* lexer) {
	this->lexer.reset(lexer);
	bool goodToken = false;
	while (!goodToken) {
		goodToken = true;
		try {
			token = std::move(this->lexer->getNextToken());
		}
		catch (CError& e) {
			addError(e);
			goodToken = false;
		}
	}
	tokenPointer = 0;

	gen = std::make_shared<CCodeGenerator>();
}

//accept functions
std::shared_ptr<CKeywordToken> CParser::acceptKeyword(KeyWords expectedKeyword) {

	if (!isKeyword())
		throw CError(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

	if (getTokenKeyword() != expectedKeyword)
		throw CError(ErrorCodes::ExpectedToken, token->getPosition(), keywordsToStr.at(expectedKeyword));
	auto keywordToken = std::dynamic_pointer_cast<CKeywordToken>(token);
	getNextToken();
	return keywordToken;
}

std::shared_ptr<CConstToken> CParser::acceptConst(VariantType expectedVariantType) {
	if (!isConst())
		throw CError(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

	std::shared_ptr<CConstToken> constToken = std::dynamic_pointer_cast<CConstToken>(token);
	VariantType tokenVariantType = constToken->getValue()->getVariantType();

	if (tokenVariantType != expectedVariantType)
		throw CError(ErrorCodes::IncorrectConstType, token->getPosition(), variantTypeToStr.at(tokenVariantType));
	getNextToken();
	return constToken;
}

std::shared_ptr<CIdentToken> CParser::acceptIdent() {
	if (!isIdent())
		throw CError(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));
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
	catch (CError& e) {
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
	catch (CError& e) {
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
	catch (CError& e) {
		addError(e);
		skipTo(acceptableTokens);
	}
}

void CParser::typeDeclaration(std::shared_ptr<CScope> scope) {
	auto ident = identifier(scope);
	acceptKeyword(KeyWords::equalSy);
	auto identType = type(scope);

	if (scope->identDefinedInScope(ident->getIdent())) {
		addError(CError(ErrorCodes::IdentifierAlreadyDefined, ident->getPosition(), ident->getIdent()));
	}
	else {
		if (!scope->typeDefined(identType->getIdent())) {
			addError(CError(ErrorCodes::IdentifierNotDefined, identType->getPosition(), identType->getIdent()));
		}
		else {
			auto tType = scope->getTypeforType(identType->getIdent());
			scope->addType(ident->getIdent(), tType);
		}
	}
}

std::shared_ptr<CIdentToken> CParser::type(std::shared_ptr<CScope> scope) {
	if (isEOF())
		throw CError(ErrorCodes::UnexpectedToken, token->getPosition(), "EOF");

	return identifier(scope);
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
	catch (CError& e) {
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
			throw CError(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

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
		addError(CError(ErrorCodes::IdentifierNotDefined, identType->getPosition(), identType->getIdent()));
	}
	else {
		for (auto ident : varIdents) {
			if (scope->identDefinedInScope(ident->getIdent())) {
				addError(CError(ErrorCodes::IdentifierAlreadyDefined, ident->getPosition(), ident->getIdent()));
			}
			else {
				gen->createVariable(ident->getIdent(), identType->getIdent(), scope);
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
	catch (CError& e) {
		addError(e);
		skipTo(acceptableTokens);
	}
}

//Okay now comes a hard part, it is time to do everything UP!

void CParser::functionDeclaration(std::shared_ptr<CScope> scope) {
	auto functionScope = std::make_shared<CScope>(scope);
	//auto [functionIdent, functionType, functionParameters] = functionHeading(functionScope);
	auto funcHeading = functionHeading(functionScope);
	auto funcIdent = funcHeading.ident;
	auto funcType = funcHeading.identType;
	auto funcParameters = funcHeading.parameters;

	bool functionOk = true;
	if (scope->identDefinedInScope(funcIdent->getIdent())) {
		addError(CError(ErrorCodes::IdentifierAlreadyDefined, funcIdent->getPosition(), funcIdent->getIdent()));
		functionOk = false;
	}
	if (!scope->typeDefined(funcType->getIdent())) {
		addError(CError(ErrorCodes::IdentifierNotDefined, funcType->getPosition(), funcType->getIdent()));
		functionOk = false;
	}

	bool paramsOk = true;
	for (int i = 0; i < funcParameters->parameters.size(); i++) {
		if (funcParameters->parameters[i]->getType() == ExprType::eErrorType) {
			paramsOk = false;
			break;
		}
	}

	if (!paramsOk) {
		addError(CError(ErrorCodes::IncorrectParameters, funcIdent->getPosition(), funcIdent->getIdent()));
	}

	if (!functionOk || !paramsOk) {
		block(functionScope);
		return;
	}


	auto function = gen->initFunction(
		funcIdent->getIdent(),
		gen->convertToTypePtr(scope->getTypeforType(funcType->getIdent())),
		funcParameters);

	scope->addFunction(funcIdent->getIdent(), funcType->getIdent(), funcParameters);
	gen->createVariable(funcIdent->getIdent(), funcType->getIdent(), functionScope);

	//define a block and create allocas
	auto parentBlock = gen->getInsertionBlock();
	auto body = gen->createBlock(function);

	gen->setInsertionPoint(body);
	gen->initFunctionParams(function, funcParameters, functionScope);

	try {
		block(functionScope);
	}
	catch (CError err) {
		function->eraseFromParent();
	}

	gen->createReturn(function, gen->getValue(funcIdent->getIdent(), functionScope->getAlloca(funcIdent->getIdent())));
	
	verifyFunction(*function);
	
	//setup previous block to insert
	gen->setInsertionPoint(parentBlock);
}


CParser::FuncHeading CParser::functionHeading(std::shared_ptr<CScope> scope) {
	acceptKeyword(KeyWords::functionSy);
	auto functionIdent = identifier(scope);
	auto functionParameters = std::make_shared<CFuncParameters>();

	if (!isKeyword())
		throw CError(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

	if (keywordEquals(KeyWords::colonSy)) {
		acceptKeyword(KeyWords::colonSy);
		auto functionType = type(scope);
		if (!scope->typeDefined(functionType->getIdent())) {
			addError(CError(ErrorCodes::IdentifierNotDefined, functionType->getPosition(), functionType->getIdent()));
		}

		acceptKeyword(KeyWords::semicolonSy);
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
			addError(CError(ErrorCodes::IdentifierNotDefined, functionType->getPosition(), functionType->getIdent()));
		}
		acceptKeyword(KeyWords::semicolonSy);

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
			throw CError(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

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
		addError(CError(ErrorCodes::IdentifierNotDefined, identType->getPosition(), identType->getIdent()));
		pType = ExprType::eErrorType;
	}
	else {
		pType = scope->getTypeforType(identType->getIdent());
	}

	for (auto ident : varIdents) {
		if (scope->identDefinedInScope(ident->getIdent())) {
			addError(CError(ErrorCodes::IdentifierAlreadyDefined, ident->getPosition(), ident->getIdent()));
		}

		parameters->addParameter(std::make_shared<CParameter>(ident->getIdent(), identType->getIdent(), pType, byRef));
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
	catch (CError& e) {
		addError(e);
		skipTo(acceptableTokens);
	}
}

void CParser::simpleStatement(std::shared_ptr<CScope> scope) {
	if (!isIdent())
		throw CError(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

	acceptIdent();
	if (!isKeyword())
		throw CError(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

	if (keywordEquals(KeyWords::assignSy)) {
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
	auto var = identifier(scope);

	if (!scope->identDefinedGlobal(var->getIdent())) {
		addError(CError(ErrorCodes::IdentifierNotDefined, var->getPosition(), var->getIdent()));
		return;
	}

	acceptKeyword(KeyWords::assignSy);
	Value* expr = expression(scope);

	auto varType = scope->getIdentType(var->getIdent());

	if (!gen->isDerived(varType, gen->convertToExprType(expr))) {
		addError(CError(ErrorCodes::TypeMismatch, pos,
			exprTypeToStr.at(scope->getIdentType(var->getIdent())) + " to " + exprTypeToStr.at(gen->convertToExprType(expr))));
		return;
	}

	gen->createAssignment(var->getIdent(), expr, scope);

}

Value* CParser::variable(std::shared_ptr<CScope> scope) {
	auto ident = identifier(scope);
	if (!scope->identDefinedGlobal(ident->getIdent())) {
		addError(CError(ErrorCodes::IdentifierNotDefined, ident->getPosition(), ident->getIdent()));
		return nullptr;
	}

	auto varName = ident->getIdent();
	return gen->getValue(varName, scope->getAlloca(varName));
}

Value* CParser::expression(std::shared_ptr<CScope> scope) {
	auto pos = token->getPosition();
	Value* leftExpr = simpleExpression(scope);

	if (isKeyword() && isRelationOperator()) {
		auto operation = getTokenKeyword();
		acceptKeyword(getTokenKeyword());
		Value* rightExpr = simpleExpression(scope);

		if (!leftExpr || !rightExpr)
			return nullptr;

		auto leftExprType = gen->convertToExprType(leftExpr);
		auto rightExprType = gen->convertToExprType(rightExpr);

		if (!gen->isDerived(leftExpr, rightExpr)) {
			addError(CError(ErrorCodes::TypeMismatch, pos,
				exprTypeToStr.at(leftExprType) + " to " + exprTypeToStr.at(rightExprType)));
			return nullptr;
		}


		if (operation == KeyWords::greaterEqualSy || operation == KeyWords::greaterSy) {
			std::swap(leftExpr, rightExpr);
			std::swap(leftExprType, rightExprType);
		}

		if (operation == KeyWords::lessSy || operation == KeyWords::greaterSy) {
			return gen->createLess(leftExpr, rightExpr);
		}

		if (operation == KeyWords::lessEqualSy || operation == KeyWords::greaterEqualSy) {
			return gen->createLessEqual(leftExpr, rightExpr);
		}

		if (operation == KeyWords::equalSy) {
			return gen->createEqual(leftExpr, rightExpr);
		}
		if (operation == KeyWords::notEqualSy) {
			return gen->createNotEqual(leftExpr, rightExpr);
		}

	}

	return leftExpr;
}

Value* CParser::simpleExpression(std::shared_ptr<CScope> scope) {
	auto pos = token->getPosition();
	Value* leftTerm = term(scope);

	while (isKeyword() && isAddingOperator()) {

		auto operation = getTokenKeyword();
		acceptKeyword(getTokenKeyword());
		Value* rightTerm = term(scope);

		if (!leftTerm || !rightTerm)
			continue;

		auto leftTermType = gen->convertToExprType(leftTerm);
		auto rightTermType = gen->convertToExprType(rightTerm);

		if (!gen->isDerived(leftTerm, rightTerm)) {
			addError(CError(ErrorCodes::TypeMismatch, pos,
				exprTypeToStr.at(leftTermType) + " to " + exprTypeToStr.at(rightTermType)));
			leftTerm = nullptr;
		}
		else {
			//can't sum boolean
			if (operation == KeyWords::plusSy) {
				if (leftTermType == ExprType::eBooleanType || rightTermType == ExprType::eBooleanType) {
					addError(CError(ErrorCodes::TypeMismatch, pos,
						exprTypeToStr.at(leftTermType) + " to " + exprTypeToStr.at(rightTermType)));
					leftTerm = nullptr;
				}
				else {
					leftTerm = gen->createAdd(leftTerm, leftTerm);
				}
			}

			//can't sub boolean
			if (operation == KeyWords::minusSy) {
				if (leftTermType == ExprType::eBooleanType || rightTermType == ExprType::eBooleanType) {
					addError(CError(ErrorCodes::TypeMismatch, pos,
						exprTypeToStr.at(leftTermType) + " to " + exprTypeToStr.at(rightTermType)));
					leftTerm = nullptr;
				}
				else {
					leftTerm = gen->createSub(leftTerm, leftTerm);
				}
			}

			//should be bool
			if (operation == KeyWords::orSy) {
				if (!(leftTermType == ExprType::eBooleanType && rightTermType == ExprType::eBooleanType)) {
					addError(CError(ErrorCodes::TypeMismatch, pos,
						exprTypeToStr.at(leftTermType) + " to " + exprTypeToStr.at(rightTermType)));
					leftTerm = nullptr;
				}
				else {
					leftTerm = gen->createOr(leftTerm, rightTerm);
				}
			}
		}
	}

	return leftTerm;
}


Value* CParser::term(std::shared_ptr<CScope> scope) {
	auto pos = token->getPosition();
	Value* leftFactor = factor(scope);

	while (isKeyword() && isMultiplyingOperator()) {
		auto operation = getTokenKeyword();
		acceptKeyword(getTokenKeyword());
		Value* rightFactor = factor(scope);

		if (!leftFactor || !rightFactor)
			continue;

		auto leftFactorType = gen->convertToExprType(leftFactor);
		auto rightFactorType = gen->convertToExprType(rightFactor);

		if (!gen->isDerived(leftFactor, rightFactor)) {
			addError(CError(ErrorCodes::TypeMismatch, pos,
				exprTypeToStr.at(leftFactorType) + " to " + exprTypeToStr.at(rightFactorType)));
			leftFactor = nullptr;
		}
		else {
			//can't 'multiply' boolean
			if (operation == KeyWords::multiplySy) {
				if (leftFactorType == ExprType::eBooleanType || rightFactorType == ExprType::eBooleanType) {
					addError(CError(ErrorCodes::TypeMismatch, pos,
						exprTypeToStr.at(leftFactorType) + " to " + exprTypeToStr.at(rightFactorType)));
					leftFactor = nullptr;
				}
				else {
					leftFactor = gen->createMult(leftFactor, rightFactor);
				}
			}
			//can't 'divide' boolean
			if (operation == KeyWords::divisionSy) {
				if (leftFactorType == ExprType::eBooleanType || rightFactorType == ExprType::eBooleanType) {
					addError(CError(ErrorCodes::TypeMismatch, pos,
						exprTypeToStr.at(leftFactorType) + " to " + exprTypeToStr.at(rightFactorType)));
					leftFactor = nullptr;
				}
				else {
					leftFactor = gen->createDivide(leftFactor, rightFactor);
				}
			}
			//can only and boolean
			if (operation == KeyWords::andSy) {
				if (!(leftFactorType == ExprType::eBooleanType && rightFactorType == ExprType::eBooleanType)) {
					addError(CError(ErrorCodes::TypeMismatch, pos,
						exprTypeToStr.at(leftFactorType) + " to " + exprTypeToStr.at(rightFactorType)));
					leftFactor = nullptr;
				}
				else {
					leftFactor = gen->createAdd(leftFactor, rightFactor);
				}
			}
		}
	}

	return leftFactor;
}

Value* CParser::factor(std::shared_ptr<CScope> scope) { //bool isUnary?
	if (isUnaryOperator()) {

		auto op = acceptKeyword(getTokenKeyword());
		auto pos = op->getPosition();
		auto value = factor(scope);

		if (!value)
			return nullptr;

		if (op->getKeyword() == KeyWords::notSy) {
			if (gen->convertToExprType(value) == ExprType::eBooleanType) {
				return gen->createNot(value);
			}
			else {
				addError(CError(ErrorCodes::TypeMismatch, pos, "not can be applied only for boolean types"));
				return nullptr;
			}
		}
		else {
			if (gen->convertToExprType(value) == ExprType::eBooleanType) {
				addError(CError(ErrorCodes::TypeMismatch, pos, "Can't apply to boolean type"));
				return nullptr;
			}
			else {
				if (op->getKeyword() == KeyWords::plusSy)
					return value;
				else
					return gen->createSub(gen->getConstInt(std::make_shared<CIntVariant>(0)), value);
			}
		}
	}

	if (isConst()) {
		auto constToken = acceptConst(getTokenVariantType());
		auto variantType = constToken->getValue()->getVariantType();
		auto exprType = variantTypeToExprType.at(variantType);
		if (exprType == ExprType::eIntType)
			return gen->getConstInt(std::dynamic_pointer_cast<CIntVariant>(constToken));
		if (exprType == ExprType::eRealType)
			return gen->getConstReal(std::dynamic_pointer_cast<CRealVariant>(constToken));
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
			addError(CError(ErrorCodes::IdentifierNotDefined, ident->getPosition(), ident->getIdent()));
			return nullptr;
		}
	}

	acceptKeyword(KeyWords::leftParSy);
	Value* exprType = expression(scope);
	acceptKeyword(KeyWords::rightParSy);
	return exprType;
}

//TODO (I should call function now!)
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
		addError(CError(ErrorCodes::IdentifierNotDefined, functionIdent->getPosition(), functionIdent->getIdent()));
		return;
	}

	if (!compareParams(parameters, scope->getFunctionParameters(functionIdent->getIdent()))) {
		addError(CError(ErrorCodes::IncorrectParameters, pos, ""));
	}

}

//TODO I should call fucntion now and verify parameters and so on
Value* CParser::functionDesignator(std::shared_ptr<CScope> scope) {
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
		addError(CError(ErrorCodes::IdentifierNotDefined, functionIdent->getPosition(), functionIdent->getIdent()));
		return ExprType::eErrorType;
	}

	if (!compareParams(parameters, scope->getFunctionParameters(functionIdent->getIdent()))) {
		addError(CError(ErrorCodes::IncorrectParameters, pos, ""));
	}
	return scope->getIdentType(functionIdent->getIdent());
}

ExprType CParser::actualParameter(std::shared_ptr<CScope> scope) {
	return expression(scope);
}

void CParser::structuredStatement(std::shared_ptr<CScope> scope) {
	if (!isKeyword())
		throw CError(ErrorCodes::UnexpectedtTokenType, token->getPosition(), tokenTypeToStr.at(token->getType()));

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

	throw CError(ErrorCodes::UnexpectedKeyword, token->getPosition(), keywordsToStr.at(getTokenKeyword()));
}

//TODO
void CParser::ifStatement(std::shared_ptr<CScope> scope) {
	acceptKeyword(KeyWords::ifSy);
	auto pos = token->getPosition();
	ExprType exprType = expression(scope);
	if (!isDerived(exprType, ExprType::eBooleanType)) {
		addError(CError(ErrorCodes::IncorrectExprType, pos, exprTypeToStr.at(exprType)));
	}

	acceptKeyword(KeyWords::thenSy);
	statement(scope);
	if (isKeyword() && keywordEquals(KeyWords::elseSy)) {
		acceptKeyword(KeyWords::elseSy);
		statement(scope);
	}
}

//TODO
void CParser::whileStatement(std::shared_ptr<CScope> scope) {
	acceptKeyword(KeyWords::whileSy);
	auto pos = token->getPosition();
	ExprType exprType = expression(scope);
	if (!isDerived(exprType, ExprType::eBooleanType)) {
		addError(CError(ErrorCodes::IncorrectExprType, pos, exprTypeToStr.at(exprType)));
	}
	acceptKeyword(KeyWords::doSy);
	statement(scope);
}


