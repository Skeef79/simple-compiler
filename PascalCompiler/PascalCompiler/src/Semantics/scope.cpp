#include "scope.h"

CScope::CScope(CScope* parent) {
	this->parent.reset(parent);
}

std::shared_ptr<CScope> CScope::getIdentScope(std::string ident) {
	
	auto currentScope = shared_from_this();

	while (currentScope && !currentScope->idents.count(ident))
		currentScope = currentScope->parent;

	return currentScope;
}

ExprType CScope::getIdentType(std::string ident) {
	auto scope = getIdentScope(ident);
	if (!scope)
		return ExprType::eErrorType;
	return scope->types[scope->idents[ident]];
}

std::shared_ptr<CFuncParameters> CScope::getFunctionParameters(std::string ident) {
	return functionParameters[ident];
}

bool CScope::checkIdent(std::string ident) {
	auto scope = getIdentScope(ident);
	if (!scope)
		return true;
	return false;
}

void CScope::addIdent(std::string ident, std::string identType) {
	idents[ident] = identType;
}

void CScope::addFunction(std::string ident, std::string functionType, std::shared_ptr<CFuncParameters> fParams) {
	idents[ident] = functionType;
	
	functionParameters[ident] = fParams;
}

void CScope::addType(std::string ident, ExprType exprType) {
	types[ident] = exprType;
}




