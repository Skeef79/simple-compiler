#pragma once
#include"llvm/ADT/APFloat.h"
#include"llvm/ADT/Optional.h"
#include"llvm/ADT/STLExtras.h"
#include"llvm/IR/BasicBlock.h"
#include"llvm/IR/Constants.h"
#include"llvm/IR/DerivedTypes.h"
#include"llvm/IR/Function.h"
#include"llvm/IR/IRBuilder.h"
#include"llvm/IR/Instructions.h"
#include"llvm/IR/LLVMContext.h"
#include"llvm/IR/LegacyPassManager.h"
#include"llvm/IR/Module.h"
#include"llvm/IR/Type.h"
#include"llvm/IR/Verifier.h"
#include"llvm/MC/TargetRegistry.h"
#include"llvm/Support/FileSystem.h"
#include"llvm/Support/Host.h"
#include"llvm/Support/TargetSelect.h"
#include"llvm/Support/raw_ostream.h"
#include"llvm/Target/TargetMachine.h"
#include"llvm/Target/TargetOptions.h"
#include "../Semantics/scope.h"
#include<algorithm>
#include<cstdlib>
#include<memory>
#include<string>
#include<vector>
#include<iostream>

using namespace llvm;
using namespace llvm::sys;

class CCodeGenerator {
public:
	CCodeGenerator();

	void printCode();

	int compileObjectTarget(const char* filename);

	bool isDerived(Value* leftValue, Value* rightValue);
	bool isDerived(ExprType leftValue, ExprType rightValue);

	void createVariable(std::string varName, std::string varType, std::shared_ptr<CScope>& scope);
	void createAssignment(std::string varName, Value* value, std::shared_ptr<CScope>& scope);


	Value* getValue(std::string varName, AllocaInst* alloca);
	Value* getConstInt(std::shared_ptr<CIntVariant> value);
	Value* getConstReal(std::shared_ptr<CRealVariant>value);

	ExprType convertToExprType(Value* value);
	Type* convertToTypePtr(ExprType exprType);

	Value* createAdd(Value* leftValue, Value* rightValue);
	Value* createSub(Value* leftValue, Value* rightValue);
	Value* createMult(Value* leftValue, Value* rightValue);
	Value* createDivide(Value* leftValue, Value* rightValue);

	Value* createAnd(Value* leftValue, Value* rightValue);
	Value* createOr(Value* leftValue, Value* rightValue);
	Value* createNot(Value* value);
	Value* createLess(Value* leftValue, Value* rightValue);
	Value* createLessEqual(Value* leftValue, Value* rightValue);
	Value* createEqual(Value* leftValue, Value* rightValue);
	Value* createNotEqual(Value* leftValue, Value* rightValue);

	Function* initFunction(std::string funcName, Type* funcType, std::shared_ptr<CFuncParameters> funcParameters);
	BasicBlock* createBlock(Function* function);
	void setInsertionPoint(BasicBlock* block);
	void initFunctionParams(Function* function, std::shared_ptr<CFuncParameters>funcParameters, std::shared_ptr<CScope> scope);
	BasicBlock* getInsertionBlock();
	void createReturn(Function* function, Value* value);


private:
	std::unique_ptr<LLVMContext> context;
	std::unique_ptr<Module> module;
	std::unique_ptr<IRBuilder<>> builder;

	std::map<ExprType, Type*> exprTypeToTypePtr;
	std::map<Type::TypeID, ExprType> typePtrToExprType;

	void initTypes();

	AllocaInst* createEntryBlockAlloca(Function* function,
		const std::string& varName, Type* varType);
};



bool compareParams(std::vector<ExprType> actualParameters, std::shared_ptr<CFuncParameters> expectedParameters);



