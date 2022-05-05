#include "error.h"
#include<iostream>

CError::CError(ErrorCodes errorCode, TextPosition pos, std::string token)
	:errorCode(errorCode), pos(pos), token(token) {
	
	message = errorMessages.at(errorCode);
	errorStr = std::to_string(pos.lineIndex + 1)
		+ ":"
		+ std::to_string(pos.charIndex + 1)
		+ ": "
		+ std::to_string(static_cast<int>(errorCode))
		+ " "
		+ message
		+ " "
		+ token;
}

const char* CError::what() const throw() {
	return errorStr.c_str();
}
