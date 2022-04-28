#include<fstream>
#include<iostream>
#include<sstream>
#include<vector>
#include<string>
#include<iomanip>
#include<sstream>

#include "Parser/parser.h"


//PascalCompiler.exe file.pas
int main(int argc, char** argv) {

	std::string fileName(argv[1]);

	std::cout << fileName << std::endl;

	//std::string fileName = "tests/Semantic/incorrect2.pas";

	std::ifstream fstream(fileName);

	std::unique_ptr<Reader> reader = std::make_unique<Reader>(fstream);
	std::unique_ptr<CLexer> lexer = std::make_unique<CLexer>(reader.release());
	std::unique_ptr<CParser> parser = std::make_unique<CParser>(lexer.release());

	parser->parse();

	std::unique_ptr<Writer> writer = std::make_unique<Writer>(std::cout);

	if (parser->errorList.empty()) {
		writer->write("Compiled succesfully!\n");
	}
	else {
		writer->write("One or more errors occured:\n");
		for (auto& e : parser->errorList) {
			writer->writeError(e);
		};
	}

	fstream.close();
}