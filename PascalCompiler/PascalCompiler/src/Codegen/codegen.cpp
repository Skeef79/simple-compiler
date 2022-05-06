#include "codegen.h"

CCodeGenerator::CCodeGenerator() {
	context = std::make_unique<LLVMContext>();
	module = std::make_unique<Module>("pascal program", *context);
	builder = std::make_unique<IRBuilder<>>(*context);
}

void CCodeGenerator::initTypes() {
	exprTypeToTypePtr[ExprType::eBooleanType] = builder->getInt1Ty();
	typePtrToExprType[builder->getInt1Ty()->getTypeID()] = ExprType::eBooleanType;

	exprTypeToTypePtr[ExprType::eIntType] = builder->getInt32Ty();
	typePtrToExprType[builder->getInt32Ty()->getTypeID()] = ExprType::eIntType;

	exprTypeToTypePtr[ExprType::eRealType] = builder->getDoubleTy();
	typePtrToExprType[builder->getDoubleTy()->getTypeID()] = ExprType::eRealType;

}

Type* CCodeGenerator::convertToTypePtr(ExprType exprType) {
	return exprTypeToTypePtr.at(exprType);
}

ExprType CCodeGenerator::convertToExprType(Value* value) {
	return typePtrToExprType.at(value->getType()->getTypeID());
}



void CCodeGenerator::printCode() {
	module->print(errs(), nullptr);
}

int CCodeGenerator::compileObjectTarget(const char* filename) {
	InitializeAllTargetInfos();
	InitializeAllTargets();
	InitializeAllTargetMCs();
	InitializeAllAsmParsers();
	InitializeAllAsmPrinters();

	auto targetTriple = sys::getDefaultTargetTriple();
	module->setTargetTriple(targetTriple);

	std::string Error;
	auto Target = TargetRegistry::lookupTarget(targetTriple, Error);

	if (!Target) {
		errs() << Error;
		return 1;
	}

	auto CPU = "generic";
	auto Features = "";

	TargetOptions opt;
	auto RM = Optional<Reloc::Model>();
	auto targetMachine =
		Target->createTargetMachine(targetTriple, CPU, Features, opt, RM);

	module->setDataLayout(targetMachine->createDataLayout());

	std::error_code EC;
	raw_fd_ostream dest(filename, EC, sys::fs::OF_None);

	if (EC) {
		errs() << "Could not open file: " << EC.message();
		return 1;
	}

	legacy::PassManager pass;
	auto fileType = CGFT_ObjectFile;

	if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
		errs() << "TheTargetMachine can't emit a file of this type";
		return 1;
	}

	pass.run(*module);
	dest.flush();

	return 0;
}

AllocaInst* CCodeGenerator::createEntryBlockAlloca(Function* function,
	const std::string& varName, Type* varType) {
	IRBuilder<> tempBlock(&function->getEntryBlock(), function->getEntryBlock().begin());
	return tempBlock.CreateAlloca(varType, nullptr,
		varName.c_str());
}

//create variable in scope, adds alloca and initialize with default value
void CCodeGenerator::createVariable(std::string varName, std::string varType, std::shared_ptr<CScope>& scope) {
	scope->addIdent(varName, varType);
	ExprType eVarType = scope->getIdentType(varName);

	if (eVarType == ExprType::eErrorType)
		return;

	//using current insertion block get the function, corresponding to it
	auto scopeFunction = builder->GetInsertBlock()->getParent();

	//create an alloca instruction in the entry block of the function
	//%varName = alloca type
	auto alloca = createEntryBlockAlloca(scopeFunction, varName, convertToTypePtr(eVarType));

	//store alloca adress in current scope
	scope->addAlloca(varName, alloca);


	//init with default value
	if (eVarType == ExprType::eIntType
		|| eVarType == ExprType::eBooleanType) {
		auto initialValue = ConstantInt::get(convertToTypePtr(eVarType), 0);
		builder->CreateStore(initialValue, alloca);
	}

	if (eVarType == ExprType::eRealType) {
		auto initialValue = ConstantFP::get(*context, APFloat(0.0));
		builder->CreateStore(initialValue, alloca);
	}

	if (eVarType == ExprType::eBooleanType) {
		auto initialValue = ConstantInt::get(builder->getInt1Ty(), 1);
		builder->CreateStore(initialValue, alloca);
	}

}

Value* CCodeGenerator::getValue(std::string varName, AllocaInst* alloca) {
	return builder->CreateLoad(alloca->getAllocatedType(), alloca, varName);
}

Value* CCodeGenerator::getConstInt(std::shared_ptr<CIntVariant> value) {
	return ConstantInt::get(convertToTypePtr(ExprType::eIntType), value->getValue());
}

Value* CCodeGenerator::getConstReal(std::shared_ptr<CRealVariant>value) {
	return ConstantFP::get(*context, APFloat(value->getValue()));
}


bool CCodeGenerator::isDerived(Value* leftValue, Value* rightValue) {
	//Error types always derived
	if (!leftValue || !rightValue)
		return true;

	auto left = convertToExprType(leftValue);
	auto right = convertToExprType(rightValue);
	return isDerived(left, right);

}

bool CCodeGenerator::isDerived(ExprType left, ExprType right) {
	if (left == ExprType::eBooleanType || right == ExprType::eBooleanType) {
		return left == right;
	}

	return true;
}

Value* CCodeGenerator::createAdd(Value* leftValue, Value* rightValue) {
	if (convertToExprType(leftValue) == ExprType::eRealType || convertToExprType(rightValue) == ExprType::eRealType) {
		if (convertToExprType(leftValue) == ExprType::eIntType)
			leftValue = builder->CreateSIToFP(leftValue, convertToTypePtr(ExprType::eRealType));
		if (convertToExprType(rightValue) == ExprType::eIntType)
			rightValue = builder->CreateSIToFP(rightValue, convertToTypePtr(ExprType::eRealType));
	}

	return builder->CreateAdd(leftValue, rightValue, "addtmp");
}

Value* CCodeGenerator::createMult(Value* leftValue, Value* rightValue) {
	if (convertToExprType(leftValue) == ExprType::eRealType || convertToExprType(rightValue) == ExprType::eRealType) {
		if (convertToExprType(leftValue) == ExprType::eIntType)
			leftValue = builder->CreateSIToFP(leftValue, convertToTypePtr(ExprType::eRealType));
		if (convertToExprType(rightValue) == ExprType::eIntType)
			rightValue = builder->CreateSIToFP(rightValue, convertToTypePtr(ExprType::eRealType));
	}
	return builder->CreateMul(leftValue, rightValue, "multmp");
}

Value* CCodeGenerator::createDivide(Value* leftValue, Value* rightValue) {
	if (convertToExprType(leftValue) == ExprType::eRealType || convertToExprType(rightValue) == ExprType::eRealType) {
		if (convertToExprType(leftValue) == ExprType::eIntType)
			leftValue = builder->CreateSIToFP(leftValue, convertToTypePtr(ExprType::eRealType));
		if (convertToExprType(rightValue) == ExprType::eIntType)
			rightValue = builder->CreateSIToFP(rightValue, convertToTypePtr(ExprType::eRealType));
	}
	return builder->CreateSDiv(leftValue, rightValue, "divtmp");
}

//leftValue and rightValue should be bool
Value* CCodeGenerator::createAnd(Value* leftValue, Value* rightValue) {
	return builder->CreateAnd(leftValue, rightValue, "andtmp");
}

Value* CCodeGenerator::createSub(Value* leftValue, Value* rightValue) {
	if (convertToExprType(leftValue) == ExprType::eRealType || convertToExprType(rightValue) == ExprType::eRealType) {
		if (convertToExprType(leftValue) == ExprType::eIntType)
			leftValue = builder->CreateSIToFP(leftValue, convertToTypePtr(ExprType::eRealType));
		if (convertToExprType(rightValue) == ExprType::eIntType)
			rightValue = builder->CreateSIToFP(rightValue, convertToTypePtr(ExprType::eRealType));
	}

	return builder->CreateSub(leftValue, rightValue, "subtmp");
}

//leftValue and rightValue should be bool
Value* CCodeGenerator::createOr(Value* leftValue, Value* rightValue) {
	return builder->CreateOr(leftValue, rightValue, "ortmp");
}

Value* CCodeGenerator::createNot(Value* value) {
	return builder->CreateNot(value, "nottmp");
}


//returns i1
Value* CCodeGenerator::createLess(Value* leftValue, Value* rightValue) {
	if (convertToExprType(leftValue) == ExprType::eBooleanType) {
		return builder->CreateICmpULT(leftValue, rightValue, "ulttmp");
	}

	if (convertToExprType(leftValue) == ExprType::eRealType || convertToExprType(rightValue) == ExprType::eRealType) {
		if (convertToExprType(leftValue) == ExprType::eIntType)
			leftValue = builder->CreateSIToFP(leftValue, convertToTypePtr(ExprType::eRealType));
		if (convertToExprType(rightValue) == ExprType::eIntType)
			rightValue = builder->CreateSIToFP(rightValue, convertToTypePtr(ExprType::eRealType));
	}

	if (convertToExprType(leftValue) == ExprType::eIntType)
		return builder->CreateICmpSLT(leftValue, rightValue, "ulttmp");
	else
		return builder->CreateFCmpULT(leftValue, rightValue, "ulttmp");

}

Value* CCodeGenerator::createLessEqual(Value* leftValue, Value* rightValue) {
	if (convertToExprType(leftValue) == ExprType::eBooleanType) {
		return builder->CreateICmpULE(leftValue, rightValue, "uletmp");
	}

	if (convertToExprType(leftValue) == ExprType::eRealType || convertToExprType(rightValue) == ExprType::eRealType) {
		if (convertToExprType(leftValue) == ExprType::eIntType)
			leftValue = builder->CreateSIToFP(leftValue, convertToTypePtr(ExprType::eRealType));
		if (convertToExprType(rightValue) == ExprType::eIntType)
			rightValue = builder->CreateSIToFP(rightValue, convertToTypePtr(ExprType::eRealType));
	}

	if (convertToExprType(leftValue) == ExprType::eIntType)
		return builder->CreateICmpSLE(leftValue, rightValue, "uletmp");
	else
		return builder->CreateFCmpULE(leftValue, rightValue, "uletmp");
}

Value* CCodeGenerator::createEqual(Value* leftValue, Value* rightValue) {
	if (convertToExprType(leftValue) == ExprType::eBooleanType) {
		return builder->CreateICmpEQ(leftValue, rightValue, "eqtmp");
	}

	if (convertToExprType(leftValue) == ExprType::eRealType || convertToExprType(rightValue) == ExprType::eRealType) {
		if (convertToExprType(leftValue) == ExprType::eIntType)
			leftValue = builder->CreateSIToFP(leftValue, convertToTypePtr(ExprType::eRealType));
		if (convertToExprType(rightValue) == ExprType::eIntType)
			rightValue = builder->CreateSIToFP(rightValue, convertToTypePtr(ExprType::eRealType));
	}

	if (convertToExprType(leftValue) == ExprType::eIntType)
		return builder->CreateICmpEQ(leftValue, rightValue, "eqtmp");
	else
		return builder->CreateFCmpUEQ(leftValue, rightValue, "eqtmp");

}

Value* CCodeGenerator::createNotEqual(Value* leftValue, Value* rightValue) {
	if (convertToExprType(leftValue) == ExprType::eBooleanType) {
		return builder->CreateICmpNE(leftValue, rightValue, "netmp");
	}

	if (convertToExprType(leftValue) == ExprType::eRealType || convertToExprType(rightValue) == ExprType::eRealType) {
		if (convertToExprType(leftValue) == ExprType::eIntType)
			leftValue = builder->CreateSIToFP(leftValue, convertToTypePtr(ExprType::eRealType));
		if (convertToExprType(rightValue) == ExprType::eIntType)
			rightValue = builder->CreateSIToFP(rightValue, convertToTypePtr(ExprType::eRealType));
	}

	if (convertToExprType(leftValue) == ExprType::eIntType)
		return builder->CreateICmpNE(leftValue, rightValue, "netmp");
	else
		return builder->CreateFCmpUNE(leftValue, rightValue, "netmp");
}


//stores the new value in alloca and converts type, if necessary
void CCodeGenerator::createAssignment(std::string varName, Value* value, std::shared_ptr<CScope>& scope) {
	//auto scopeFunction = builder->GetInsertBlock()->getParent();
	auto varType = scope->getIdentType(varName);
	auto alloca = scope->getAlloca(varName);

	if (varType == ExprType::eBooleanType) {
		builder->CreateStore(value, alloca);
	}

	if (varType == ExprType::eRealType) {
		if (convertToExprType(value) == ExprType::eIntType) {
			value = builder->CreateSIToFP(value, convertToTypePtr(ExprType::eRealType));
		}
		builder->CreateStore(value, alloca);
	}
	if (varType == ExprType::eIntType) {
		if (convertToExprType(value) == ExprType::eRealType) {
			value = builder->CreateFPToSI(value, convertToTypePtr(ExprType::eIntType));
		}
		builder->CreateStore(value, alloca);
	}

}


Function* CCodeGenerator::initFunction(std::string funcName, Type* funcType, std::shared_ptr<CFuncParameters> funcParameters) {
	std::vector<Type*>paramTypes(funcParameters->parameters.size());
	for (int i = 0; i < funcParameters->parameters.size(); i++) {
		paramTypes[i] = convertToTypePtr(funcParameters->parameters[i]->getType());
	}

	auto functionType = FunctionType::get(funcType,
		paramTypes, false);
	auto function = Function::Create(functionType, Function::ExternalLinkage, funcName, module.get());
	int i = 0;
	for (auto& arg : function->args()) {
		arg.setName(funcParameters->parameters[i]->getName());
		i++;
	}

	return function;
}

BasicBlock* CCodeGenerator::createBlock(Function* function) {
	return BasicBlock::Create(*context, "entry", function);
}

void CCodeGenerator::setInsertionPoint(BasicBlock* block) {
	builder->SetInsertPoint(block);
}

//create variables for function parameters
void CCodeGenerator::initFunctionParams(Function* function, std::shared_ptr<CFuncParameters> funcParameters, std::shared_ptr<CScope> scope) {
	for (auto param : funcParameters->parameters) {
		createVariable(param->getName(), param->getStrType(), scope);
	}
}

BasicBlock* CCodeGenerator::getInsertionBlock() {
	return builder->GetInsertBlock();
}

void CCodeGenerator::createReturn(Function* function, Value* value) {
	builder->CreateRet(value);
}


