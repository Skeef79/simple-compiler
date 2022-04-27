#pragma once
#include<memory>
#include<vector>

enum class ExprType :char {
	eIntType,
	eStringType,
	eRealType,
	eBooleanType,
	eErrorType
};

class CParameter {
public:
	CParameter(ExprType pType, bool byRef);
	ExprType getType();
	bool isRef();
private:
	ExprType pType;
	bool byRef;
};

class CFuncParameters {
public:
	std::vector<std::shared_ptr<CParameter>> parameters;
	CFuncParameters(std::vector<std::shared_ptr<CParameter>> params);
};