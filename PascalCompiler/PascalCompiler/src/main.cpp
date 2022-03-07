#include<fstream>
#include<iostream>
#include<sstream>
#include<vector>
#include<string>
#include<iomanip>

#include "Parser/parser.h"
#include "../include/color.hpp"

int main(char argc, char** argv) {
	//TODO: input filename from command line arguments

	std::ifstream fstream("tests/Lexer/test1.pas");

	std::unique_ptr<Reader> reader = std::make_unique<Reader>(fstream);
	std::unique_ptr<CLexer> lexer = std::make_unique<CLexer>(reader.release());
	std::unique_ptr<CParser> parser = std::make_unique<CParser>(lexer.release());

	try {
		while (true) {
			parser->getNextToken();
			if (parser->token == nullptr)
				break;
			printToken(parser->token);
		}
	}
	catch (std::exception& e) {
		std::cerr << dye::red("error: ") << e.what();
		return -1;
	}

	
	fstream.close();

}