#include "variants.h"
#include<string>


CVariant::CVariant(VariantType variantType)
	:variantType(variantType) {}

VariantType CVariant::getVariantType() {
	return variantType;
}


CIntVariant::CIntVariant(int value)
	: CVariant(VariantType::vtInt), value(value) {}

int CIntVariant::getValue() {
	return value;
}

std::string CIntVariant::toString() {
	return std::to_string(value);
}



CRealVariant::CRealVariant(double value)
	: CVariant(VariantType::vtReal), value(value) {}

double CRealVariant::getValue() {
	return value;
}

std::string CRealVariant::toString() {
	return std::to_string(value);
}



CStringVariant::CStringVariant(std::string value)
	: CVariant(VariantType::vtString), value(value) {}

std::string CStringVariant::getValue() {
	return value;
}

std::string CStringVariant::toString() {
	return value;
}


CBooleanVariant::CBooleanVariant(bool value)
	: CVariant(VariantType::vtBoolean), value(value) {}

bool CBooleanVariant::getValue() {
	return value;
}

std::string CBooleanVariant::toString() {
	return (value ? "true" : "false");
}





