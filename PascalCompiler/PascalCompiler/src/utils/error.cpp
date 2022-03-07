#include "error.h"
#include<iostream>

Error::Error(ErrorCodes errorCode, TextPosition pos, std::string token)
	:errorCode(errorCode), pos(pos), token(token) {
	
	message = errorMessages.at(errorCode);
	errorStr = std::to_string(pos.lineIndex+1)
		+ ":"
		+ std::to_string(pos.charIndex+1)
		+ ": "
		+ "L"
		+ std::to_string(static_cast<int>(errorCode))
		+ " "
		+ message
		+ " "
		+ "\""
		+ token
		+ "\"";
}

const char* Error::what() const throw() {
	return errorStr.c_str();
}
