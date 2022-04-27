#include<fstream>
#include<iostream>
#include<sstream>
#include<vector>
#include<string>
#include<iomanip>
#include "Parser/parser.h"
#include "../include/color.hpp"


int main(int argc, char** argv) {
	//TODO: input filename from command line arguments

	std::ifstream fstream("tests/Parser/incorrect2.pas");

	std::unique_ptr<Reader> reader = std::make_unique<Reader>(fstream);
	std::unique_ptr<CLexer> lexer = std::make_unique<CLexer>(reader.release());
	std::unique_ptr<CParser> parser = std::make_unique<CParser>(lexer.release());


	parser->parse();

	for (auto& e : parser->errorList) {
		std::cerr << dye::light_red("error: ") << e.what() << std::endl;
	};

	fstream.close();

}