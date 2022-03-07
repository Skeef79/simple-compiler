#pragma once
#define EOL '\n'
#define MAX_INT_LEN 14

struct TextPosition {
	int lineIndex;
	int charIndex;

	TextPosition();
	TextPosition(int lineIndex, int charIndex);
};


