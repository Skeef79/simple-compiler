#pragma once
#include<map>
#include<string>
#include<set>

enum class KeyWords : char {
	programSy,		// program 
	beginSy,		// begin 
	endSy,			// end 
	varSy,			// var 
	typeSy,			// type 
	functionSy,		// function 
	ifSy,			// if 
	thenSy,			// then 
	elseSy,			// else 
	whileSy,		// while 
	doSy,			// do 
	andSy,			// and 
	orSy,			// or
	notSy,			// not 
	plusSy,			// +
	minusSy,		// - 
	multiplySy,		// *
	divisionSy,		// /
	leftParSy,		// (
	rightParSy,		// )
	assignSy,		// :=
	dotSy,			// .
	commaSy,		// ,
	colonSy,		// :
	semicolonSy,	// ;
	equalSy,		// =
	lessSy,			// <
	greaterSy,		// >
	lessEqualSy,	// <=
	greaterEqualSy,	// >=
	notEqualSy,		// <>
};

const std::map<std::string, KeyWords> strToKeywords = {
	{"program", KeyWords::programSy},
	{"begin", KeyWords::beginSy},
	{"end", KeyWords::endSy},
	{"var", KeyWords::varSy},
	{"type", KeyWords::typeSy},
	{"function", KeyWords::functionSy},
	{"if", KeyWords::ifSy},
	{"then", KeyWords::thenSy},
	{"else", KeyWords::elseSy},
	{"while", KeyWords::whileSy},
	{"do", KeyWords::doSy},
	{"and", KeyWords::andSy},
	{"or", KeyWords::orSy},
	{"not", KeyWords::notSy},
	{"+", KeyWords::plusSy},
	{"-", KeyWords::minusSy},
	{"*", KeyWords::multiplySy},
	{"/", KeyWords::divisionSy},
	{"(", KeyWords::leftParSy},
	{")", KeyWords::rightParSy},
	{":=", KeyWords::assignSy},
	{".", KeyWords::dotSy},
	{",", KeyWords::commaSy},
	{":", KeyWords::colonSy},
	{";", KeyWords::semicolonSy},
	{"=", KeyWords::equalSy},
	{"<", KeyWords::lessSy},
	{">", KeyWords::greaterSy},
	{"<=", KeyWords::lessEqualSy},
	{">=", KeyWords::greaterEqualSy},
	{"<>", KeyWords::notEqualSy},
};

const std::map<KeyWords, std::string> keywordsToStr = {
	{KeyWords::programSy, "program"},
	{KeyWords::beginSy, "begin"},
	{KeyWords::endSy, "end"},
	{KeyWords::varSy, "var"},
	{KeyWords::typeSy, "type"},
	{KeyWords::functionSy, "function"},
	{KeyWords::ifSy, "if"},
	{KeyWords::thenSy, "then"},
	{KeyWords::elseSy, "else"},
	{KeyWords::whileSy, "while"},
	{KeyWords::doSy, "do"},
	{KeyWords::andSy, "and"},
	{KeyWords::orSy, "or"},
	{KeyWords::notSy, "not"},
	{KeyWords::plusSy, "+"},
	{KeyWords::minusSy, "-"},
	{KeyWords::multiplySy, "*"},
	{KeyWords::divisionSy, "/"},
	{KeyWords::leftParSy, "("},
	{KeyWords::rightParSy, ")"},
	{KeyWords::assignSy, ":="},
	{KeyWords::dotSy, "."},
	{KeyWords::commaSy, ","},
	{KeyWords::colonSy, ":"},
	{KeyWords::semicolonSy, ";"},
	{KeyWords::equalSy, "="},
	{KeyWords::lessSy, "<"},
	{KeyWords::greaterSy, ">"},
	{KeyWords::lessEqualSy, "<="},
	{KeyWords::greaterEqualSy, ">=" },
	{KeyWords::notEqualSy, "<>"},
};

bool isKeyword(std::string s);

const std::set<char> oneCharKeywords = {
	'+',
	'-',
	'*',
	'/',
	'(',
	')',
	'{',
	'}',
	'.',
	',',
	';',
	'='
};

bool isOneCharKeyword(const char ch);