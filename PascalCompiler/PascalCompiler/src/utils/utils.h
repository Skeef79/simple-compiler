#define EOL '\n'

struct TextPosition {
	int lineIndex;
	int charIndex;
	TextPosition();

	TextPosition(int lineIndex, int charIndex);
};
