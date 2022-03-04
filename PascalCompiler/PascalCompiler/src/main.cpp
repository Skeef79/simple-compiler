#include<fstream>
#include<iostream>
#include<sstream>
#include "Lexer/lexer.h"

int main(char argc, char** argv) {
	//TODO: input filename from command line arguments


	/*std::string filename = "examples/example2.pas";
	std::ifstream fstream(filename);

	std::unique_ptr<Reader> reader = std::make_unique<Reader>(fstream);

	while (true) {
		auto [ch, pos] = reader->get();
		std::cout << ch << ' ' << pos.lineIndex << ' ' << pos.charIndex << std::endl;
		std::cout << ch;
		if (ch == EOF)
			break;
	}*/

	std::unique_ptr<CToken> t1 = std::make_unique<CIdentToken>("Igor");
	std::unique_ptr<CToken> t2 = std::make_unique<CIdentToken>("a3");
	std::unique_ptr<CToken> t3 = std::make_unique<CKeywordToken>(strToKeywords.at("+"));

	std::unique_ptr<CVariant> value = std::make_unique<CStringVariant>("kek");
	std::unique_ptr<CToken> t4 = std::make_unique<CConstToken>(value.release());


	auto  print = [](const std::unique_ptr<CToken>& t) {
		std::cout << t->toString() << std::endl;
	};

	print(t1);
	print(t2);
	print(t3);
	print(t4);


}