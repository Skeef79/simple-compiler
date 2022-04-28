#pragma once
#include "types.h"
#include<map>
#include<string>

class CScope : public std::enable_shared_from_this<CScope> {
public:
	CScope(std::shared_ptr<CScope> parent);
	std::shared_ptr<CScope> getIdentScope(std::string ident);
	ExprType getIdentType(std::string ident);
	std::shared_ptr<CFuncParameters> getFunctionParameters(std::string ident);
	bool identDefinedInScope(std::string ident);
	bool identDefinedGlobal(std::string ident);
	bool typeDefined(std::string ident);
	void addIdent(std::string ident, std::string identType);
	void addFunction(std::string ident, std::string functionType, std::shared_ptr<CFuncParameters> fParams);
	void addType(std::string ident, ExprType exprType);
	bool isFunction(std::string ident);

private:
	std::shared_ptr<CScope> parent;
	std::map<std::string, std::string>idents;
	std::map<std::string, ExprType> types;
	std::map<std::string, std::shared_ptr<CFuncParameters>> functions;
};

