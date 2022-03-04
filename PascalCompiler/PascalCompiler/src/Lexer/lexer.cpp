#include "lexer.h"
#include "../utils/utils.h"

CLexer::CLexer(Reader* reader) {
	this->reader.reset(reader);
}

std::unique_ptr<CToken> CLexer::getNextToken() {
	char ch;
	TextPosition pos;
	
	do {
		std::tie(ch, pos) = reader->get();
	} while (isSpace(ch));

	if (isLetter(ch)) {
		//std::string letter = getLetter();

	}
	else
		if (isdigit(ch)) {

		}

}
