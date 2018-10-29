#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "include/parser.hpp"
#include "include/runtime_a.hpp"
using namespace std;
using namespace dbas;


int main() {
	printf("-- parse begin --\n");

	// test expressions
	// Parser_Tokenize pt;  return pt.test();
	// Parser_Expression pe;  return pe.test();

	// parse
	string fname = "../testfiles/test-00.bas";
	Parser p;
	p.load(fname);
	if (p.parse()) {
		printf("-- parse failed --\n");
		return 1;
	}
	streamnodefile("output.txt", p.prog);  // save to file
	printf("-- parse OK --\n");

	// run
	RunTreeA r;
	r.prog = p.prog;
	r.run();
	r.debug_vars();
}