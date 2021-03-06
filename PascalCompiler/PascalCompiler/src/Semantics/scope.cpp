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
	auto identType = idents[ident];
	auto scope = getIdentScope(identType);
	if (!scope)
		return ExprType::eErrorType;
	return scope->types[identType];
}

ExprType CScope::getTypeforType(std::string ident) {
	auto scope = getIdentScope(ident);
	if (!scope)
		return ExprType::eErrorType;
	return scope->types[ident];
}

std::shared_ptr<CFuncParameters> CScope::getFunctionParameters(std::string ident) {
	auto currentScope = shared_from_this();
	while (currentScope && !currentScope->functions.count(ident))
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

void CScope::addFunction(std::string ident, std::string functionType, std::shared_ptr<CFuncParameters> fParams, Function* functionPtr) {
	idents[ident] = functionType;
	functions[ident] = fParams;
	functionPtrs[ident] = functionPtr;
}

Function* CScope::getFunctionPtr(std::string ident) {
	auto currentScope = shared_from_this();
	while (currentScope && !currentScope->functions.count(ident))
		currentScope = currentScope->parent;
	return currentScope->functionPtrs[ident];
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


void CScope::addAlloca(std::string ident, AllocaInst* alloca) {
	varAllocas[ident] = alloca;
}

AllocaInst* CScope::getAlloca(std::string ident) {
	auto currentScope = shared_from_this();
	while (currentScope && !currentScope->idents.count(ident))
		currentScope = currentScope->parent;

	return currentScope->varAllocas.at(ident);
}

