#pragma once
#define EOL '\n'

struct TextPosition {
	int lineIndex;
	int charIndex;
	TextPosition();

	TextPosition(int lineIndex, int charIndex);
};

bool isSpace(char ch);
bool isLetter(char ch);

