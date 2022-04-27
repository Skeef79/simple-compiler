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
