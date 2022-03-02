#include "io.h"

Reader::Reader(std::istream& stream) : stream(stream) {}

Reader::~Reader() {}

std::pair<char, TextPosition> Reader::get() {
	char ch = stream.get();

	std::pair<char, TextPosition> result = { ch,pos };
	if (ch == EOL) {
		pos.lineIndex++;
		pos.charIndex = 0;
	}
	else {
		pos.charIndex++;
	}

	return result;
}
