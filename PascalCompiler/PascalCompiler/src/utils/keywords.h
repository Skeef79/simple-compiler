#pragma once
#include<map>
#include<string>

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
	orSy,			// orSy 
	xorSy,			// xor 
	notSy,			// not 
	addressSy,		// @ 
	pointerSy,		// ^
	plusSy,			// +
	minusSy,		// - 
	multiplySy,		// *
	divisionSy,		// /
	leftParSy,		// (
	rightParSy,		// )
	leftCurlySy,	// {
	rightCurlySy,	// }
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
	leftCommentSy,	// (*
	rightCommentSy	// *)
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
	{"xor", KeyWords::xorSy},
	{"not", KeyWords::notSy},
	{"@", KeyWords::addressSy},
	{"^", KeyWords::pointerSy},
	{"+", KeyWords::plusSy},
	{"-", KeyWords::minusSy},
	{"*", KeyWords::multiplySy},
	{"/", KeyWords::divisionSy},
	{"(", KeyWords::leftParSy},
	{")", KeyWords::rightParSy},
	{"{", KeyWords::leftCurlySy},
	{"}", KeyWords::rightCurlySy},
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
	{"(*", KeyWords::leftCommentSy},
	{"*)", KeyWords::rightCommentSy}
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
	{KeyWords::xorSy, "xor"},
	{KeyWords::notSy, "not"},
	{KeyWords::addressSy, "@"},
	{KeyWords::pointerSy, "^"},
	{KeyWords::plusSy, "+"},
	{KeyWords::minusSy, "-"},
	{KeyWords::multiplySy, "*"},
	{KeyWords::divisionSy, "/"},
	{KeyWords::leftParSy, "("},
	{KeyWords::rightParSy, ")"},
	{KeyWords::leftCurlySy, "{"},
	{KeyWords::rightCurlySy, "}"},
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
	{KeyWords::leftCommentSy, "(*"},
	{KeyWords::rightCommentSy, "*)"}
};

bool isKeyword(std::string s);