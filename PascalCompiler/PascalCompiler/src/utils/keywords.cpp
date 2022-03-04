#include "keywords.h"
#include<string>

bool isKeyword(std::string s) {
	return strToKeywords.count(s);
}
