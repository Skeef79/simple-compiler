#pragma once
#include<memory>
#include<vector>
#include<string>
#include<map>
#include "../utils/variants.h"

enum class ExprType :char {
	eIntType,
	eRealType,
	eBooleanType,
	eErrorType
};

const std::map<ExprType, std::string> exprTypeToStr = {
	{ExprType::eIntType, "integer"},
	{ExprType::eRealType, "real"},
	{ExprType::eBooleanType, "bolean"},
	{ExprType::eErrorType,"error"}
};

const std::map<VariantType, ExprType> variantTypeToExprType{
	{VariantType::vtBoolean, ExprType::eBooleanType},
	{VariantType::vtInt, ExprType::eIntType},
	{VariantType::vtReal, ExprType::eRealType},
};

class CParameter {
public:
	CParameter(std::string name, std::string strType, ExprType pType, bool byRef);
	ExprType getType();
	std::string getName();
	std::string getStrType();
	bool isRef();

private:
	std::string pName;
	std::string strType;
	ExprType pType;
	bool byRef;
};

class CFuncParameters {
public:
	std::vector<std::shared_ptr<CParameter>> parameters;
	CFuncParameters(std::vector<std::shared_ptr<CParameter>> params);
	CFuncParameters();
	void addParameter(std::shared_ptr<CParameter> param);
	void addParameters(std::shared_ptr<CFuncParameters> params);
};

