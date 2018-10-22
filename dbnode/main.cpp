#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "include/parser.hpp"
using namespace std;
using namespace dbas;


int main() {
	printf("-- parse begin --\n");

	// test expressions
	// Parser_Tokenize pt;  return pt.test();
	// Parser_Expression pe;  return pe.test();

	string fname = "../testfiles/test-00.bas";
	Parser p;
	p.load(fname);
	if (p.parse()) {
		printf("-- parse failed --\n");
		return 1;
	}

	// streamnode(cout, prog);
	fstream fs("output.txt", ios::out);
	fs << __DATE__ << " -- " << __TIME__ << endl;
	fs << fname << endl;
	fs << "-----" << endl;
	streamnode(fs, p.prog);
	printf("-- parse OK --\n");
}