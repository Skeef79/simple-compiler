#pragma once
#include<map>
#include<string>

enum class VariantType {
	vtInt,
	vtReal,
	vtString,
	vtBoolean
};

const std::map<std::string, VariantType> strToVariantType = {
	{"integer", VariantType::vtInt},
	{"real", VariantType::vtReal},
	{"string", VariantType::vtString},
	{"boolean", VariantType::vtBoolean}
};

const std::map<VariantType, std::string> variantTypeToStr = {
	{VariantType::vtInt,"integer"},
	{VariantType::vtReal,"real"},
	{VariantType::vtString,"string"},
	{VariantType::vtBoolean,"boolean"}
};

class CVariant {
public:
	CVariant(VariantType variantType);
	VariantType getVariantType();
	virtual std::string toString() = 0;

private:
	VariantType variantType;
};


class CIntVariant : public CVariant {
public:
	CIntVariant(int value);
	std::string toString() override;
	int getValue();
private:
	int value;
};

class CRealVariant : public CVariant {
public:
	CRealVariant(double value);
	std::string toString() override;
	double getValue();
private:
	double value;
};

class CStringVariant : public CVariant {
public:
	CStringVariant(std::string value);
	std::string toString() override;
	std::string getValue();
private:
	std::string value;
};

class CBooleanVariant : public CVariant {
public:
	CBooleanVariant(bool value);
	std::string toString() override;
	bool getValue();
private:
	bool value;
};

