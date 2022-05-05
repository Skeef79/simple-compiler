#pragma once

#include<istream>
#include<ostream>
#include<string>
#include<utility>
#include "../utils/utils.h"
#include "../utils/error.h"

class Reader {
public:

	Reader(std::istream& stream);
	~Reader();

	std::pair<char, TextPosition> get();

private:
	std::istream& stream;
	TextPosition pos;
};

class Writer {
public:
	Writer(std::ostream& stream);
	~Writer();
	void write(std::string data);
	void writeError(CError e);

private:
	std::ostream& stream;
};
