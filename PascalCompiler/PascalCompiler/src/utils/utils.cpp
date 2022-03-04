#include "utils.h"
#include<string>

TextPosition::TextPosition() {
	lineIndex = 0;
	charIndex = 0;
}

TextPosition::TextPosition(int lineIndex, int charIndex) 
	: lineIndex(lineIndex), charIndex(charIndex) {}

bool isSpace(char ch) {
	return isspace(ch);
}

bool isLetter(char ch) {
	return ch >= 'A' && ch <= 'Z' || ch >= 'a' && ch <= 'z' || ch == '_';
}