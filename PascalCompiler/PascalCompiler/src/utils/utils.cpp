#include "utils.h"
#include<string>

TextPosition::TextPosition() {
	lineIndex = 0;
	charIndex = 0;
}

TextPosition::TextPosition(int lineIndex, int charIndex) 
	: lineIndex(lineIndex), charIndex(charIndex) {}


