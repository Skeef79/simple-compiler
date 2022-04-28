#include "scope.h"

CScope::CScope(std::shared_ptr<CScope> parent) {
	this->parent = parent;
}

std::shared_ptr<CScope> CScope::getIdentScope(std::string ident) {
	
	auto currentScope = shared_from_this();

	while (currentScope && !currentScope->types.count(ident))
		currentScope = currentScope->parent;

	return currentScope;
}

ExprType CScope::getIdentType(std::string ident) {
	auto scope = getIdentScope(idents[ident]);
	if (!scope)
		return ExprType::eErrorType;
	return scope->types[scope->idents[ident]];
}

std::shared_ptr<CFuncParameters> CScope::getFunctionParameters(std::string ident) {
	auto currentScope = shared_from_this();
	while (currentScope && !currentScope->idents.count(ident))
		currentScope = currentScope->parent;
	
	return currentScope->functions[ident];
}

bool CScope::identDefinedInScope(std::string ident) {
	return idents.count(ident);
}

bool CScope::identDefinedGlobal(std::string ident) {
	auto currentScope = shared_from_this();
	while (currentScope && !currentScope->idents.count(ident))
		currentScope = currentScope->parent;
	return (currentScope ? true : false);
}

bool CScope::typeDefined(std::string ident) {
	auto scope = getIdentScope(ident);
	return (scope ? true : false);
}

void CScope::addIdent(std::string ident, std::string identType) {
	idents[ident] = identType;
}

void CScope::addFunction(std::string ident, std::string functionType, std::shared_ptr<CFuncParameters> fParams) {
	idents[ident] = functionType;
	functions[ident] = fParams;
}

void CScope::addType(std::string ident, ExprType exprType) {
	types[ident] = exprType;
}

bool CScope::isFunction(std::string ident) {
	auto currentScope = shared_from_this();
	while (currentScope && !currentScope->functions.count(ident))
		currentScope = currentScope->parent;
	return (currentScope ? true : false);
}



