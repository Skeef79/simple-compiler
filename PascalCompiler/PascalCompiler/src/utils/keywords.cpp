#include "keywords.h"
#include<string>

bool isKeyword(std::string s) {
	return strToKeywords.count(s);
}

bool isOneCharKeyword(const char ch) {
	return oneCharKeywords.count(ch);
}