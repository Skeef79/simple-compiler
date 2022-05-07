#include "types.h"


CParameter::CParameter(std::string name, std::string strType, ExprType pType, bool byRef)
	: pName(name), strType(strType), pType(pType), byRef(byRef) {}

ExprType CParameter::getType() {
	return pType;
}

std::string CParameter::getStrType() {
	return strType;
}

bool CParameter::isRef() {
	return byRef;
}

std::string CParameter::getName() {
	return pName;
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



