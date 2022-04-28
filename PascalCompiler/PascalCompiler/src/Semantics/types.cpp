#include "types.h"


CParameter::CParameter(ExprType pType, bool byRef)
	: pType(pType), byRef(byRef) {}

ExprType CParameter::getType() {
	return pType;
}

bool CParameter::isRef() {
	return byRef;
}

CFuncParameters::CFuncParameters(std::vector<std::shared_ptr<CParameter>> params) {
	this->parameters = params;
}

CFuncParameters::CFuncParameters() {}

void CFuncParameters::addParameter(std::shared_ptr<CParameter> param) {
	parameters.push_back(param);
}

void CFuncParameters::addParameters(std::shared_ptr<CFuncParameters> params) {
	for (auto param : params->parameters) {
		addParameter(param);
	}
}

bool compareParams(std::vector<ExprType> actualParameters, std::shared_ptr<CFuncParameters> expectedParameters) {
	if (actualParameters.size() != expectedParameters->parameters.size())
		return false;
	for (int i = 0; i < actualParameters.size(); i++) {
		if (actualParameters[i] != expectedParameters->parameters[i]->getType())
			return false;
	}
	return true;
}

//strong typing!!!
bool isDerived(ExprType left, ExprType right) {
	//Error types always derived
	if (left == ExprType::eErrorType || right == ExprType::eErrorType)
		return true;

	return left == right;
}
