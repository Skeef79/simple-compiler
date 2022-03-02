#include<fstream>
#include<iostream>
#include<sstream>
#include "IO/io.h"

int main(char argc, char** argv) {
	//TODO: input filename from command line arguments

	std::string filename = "examples/example2.pas";
	std::ifstream fstream(filename);

	std::unique_ptr<Reader> reader = std::make_unique<Reader>(fstream);

	while (true) {
		auto [ch, pos] = reader->get();
		//std::cout << ch << ' ' << pos.lineIndex << ' ' << pos.charIndex << std::endl;
		std::cout << ch;
		if (ch == EOF)
			break;
	}


}