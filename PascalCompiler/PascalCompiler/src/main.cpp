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

	//std::string fileName(argv[1]);

	//std::cout << fileName << std::endl;


	std::string fileName = "tests/Semantic/simple.pas";

	std::ifstream fstream(fileName);

	if (fstream.fail()) {
		std::cout << "Can't open: " + fileName;
		return 1;
	}

	std::unique_ptr<Reader> reader = std::make_unique<Reader>(fstream);
	std::unique_ptr<CLexer> lexer = std::make_unique<CLexer>(reader.release());
	std::unique_ptr<CParser> parser = std::make_unique<CParser>(lexer.release());

	parser->parse();

	std::unique_ptr<Writer> writer = std::make_unique<Writer>(std::cout);

	if (parser->errorList.empty()) {
		writer->write("Compiled succesfully!\n");
		parser->printGeneratedCode();
		parser->compile("result.o");
	}
	else {
		writer->write("One or more errors occured:\n");
		for (auto& e : parser->errorList) {
			writer->writeError(e);
		};
	}

	fstream.close();
}