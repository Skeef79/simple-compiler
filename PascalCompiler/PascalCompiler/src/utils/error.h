#pragma once
#include<exception>
#include "../utils/utils.h"
#include<string>
#include<map>

enum class ErrorCodes {
	IntegerOverflow = 101,
	UnknownSymbol = 201,
	IncorrectNumber = 102,
	UnexpectedSymbol = 202,
	NoMatchingSymbol = 301,
	ExpectedToken = 401,
	IncorrectConstType = 103,
	UnexpectedtTokenType = 402,
	UnexpectedKeyword = 403
};

const std::map<ErrorCodes, std::string> errorMessages = {
	{ErrorCodes::IntegerOverflow, "Integer constant is too large"},
	{ErrorCodes::UnknownSymbol, "Unknown symbol"},
	{ErrorCodes::IncorrectNumber, "Number is incorrect"},
	{ErrorCodes::NoMatchingSymbol, "No matching symbol found"},
	{ErrorCodes::UnexpectedSymbol, "Unexpected symbol"},
	{ErrorCodes::ExpectedToken, "Expected token"},
	{ErrorCodes::IncorrectConstType, "Incorrect const type"},
	{ErrorCodes::UnexpectedtTokenType, "Unexpected token type"},
	{ErrorCodes::UnexpectedKeyword, "Unexpected keyword"}
};

//add filename?
class Error : public std::exception {
public:
	explicit Error(ErrorCodes errorCode, TextPosition pos, std::string token);
	const char* what() const throw() override;

private:
	std::string message;
	ErrorCodes errorCode;
	TextPosition pos;
	std::string token;
	std::string errorStr;
};