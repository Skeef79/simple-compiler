#pragma once
#include "types.h"
#include<map>
#include<string>
#include "llvm/IR/Instructions.h"

using namespace llvm;
using namespace llvm::sys;


class CScope : public std::enable_shared_from_this<CScope> {
public:
	CScope(std::shared_ptr<CScope> parent);
	std::shared_ptr<CScope> getIdentScope(std::string ident);
	ExprType getIdentType(std::string ident);
	ExprType getTypeforType(std::string ident);

	std::shared_ptr<CFuncParameters> getFunctionParameters(std::string ident);
	bool identDefinedInScope(std::string ident);
	bool identDefinedGlobal(std::string ident);
	bool typeDefined(std::string ident);
	void addIdent(std::string ident, std::string identType);
	void addFunction(std::string ident, 
		std::string functionType, 
		std::shared_ptr<CFuncParameters> fParams,
		Function* functinPtr);
	bool isFunction(std::string ident);
	Function* getFunctionPtr(std::string ident);

	void addType(std::string ident, ExprType exprType);
	void addAlloca(std::string ident, AllocaInst* alloca);
	AllocaInst* getAlloca(std::string ident);

private:
	std::shared_ptr<CScope> parent;
	std::map<std::string, std::string>idents;
	std::map<std::string, ExprType> types;
	std::map<std::string, std::shared_ptr<CFuncParameters>> functions;
	std::map<std::string, Function*> functionPtrs;

	//store allocas to variables (names same as in idents)
	std::map<std::string, AllocaInst*> varAllocas;

};


