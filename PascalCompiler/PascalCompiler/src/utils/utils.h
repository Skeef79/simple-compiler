#define EOL '\n'

struct TextPosition {
	int lineIndex;
	int charIndex;
	TextPosition() {
		lineIndex = 0;
		charIndex = 0;
	}

	TextPosition(int lineIndex, int charIndex) : lineIndex(lineIndex), charIndex(charIndex) {}
};
