#pragma once

#include<istream>
#include<ostream>
#include<string>
#include<utility>
#include "../utils/utils.h"

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
	Writer(const std::istream& stream);
	~Writer();


private:
	std::istream& stream;
};
