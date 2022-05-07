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
	int getTypeHash(Type* t);

	bool isDerived(Value* leftValue, Value* rightValue);
	bool isDerived(ExprType leftValue, ExprType rightValue);

	void createVariable(std::string varName, std::string varType, std::shared_ptr<CScope>& scope);
	void createAssignment(std::string varName, Value* value, std::shared_ptr<CScope>& scope);


	Value* getValue(std::string varName, AllocaInst* alloca);
	Value* getConstInt(std::shared_ptr<CIntVariant> value);
	Value* getConstReal(std::shared_ptr<CRealVariant>value);
	Value* getTrue();
	Value* getFalse();

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
	BasicBlock* createBlock(Function* function,std::string name = "entry");
	BasicBlock* createBlock(std::string name);
	void setInsertionPoint(BasicBlock* block);
	void initFunctionParams(Function* function, std::shared_ptr<CFuncParameters>funcParameters, std::shared_ptr<CScope> scope);
	BasicBlock* getInsertionBlock();
	void createReturn(Function* function, Value* value);
	Value* createCall(Function* function, std::vector<Value*>parameters);
	Function* getCurrentFunction();
	bool compareParams(std::vector<Value*> actualParameters, std::shared_ptr<CFuncParameters> expectedParameters);
	void createCondBr(Value* cond, BasicBlock* thenBlock, BasicBlock* elseBlock);
	void createBr(BasicBlock* block);
	void addBlock(Function* function, BasicBlock* block);
	void initWrite(std::shared_ptr<CScope>scope);

	std::map<ExprType, Type*> exprTypeToTypePtr;
private:

	std::unique_ptr<LLVMContext> context;
	std::unique_ptr<Module> module;
	std::unique_ptr<IRBuilder<>> builder;

	std::map<int, ExprType> typePtrToExprType;

	void initTypes();

	AllocaInst* createEntryBlockAlloca(Function* function,
		const std::string& varName, Type* varType);
};





