#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "include/parser.hpp"
using namespace std;
using namespace dbas;


int main() {
	printf("hello world\n");

	string fname = "../testfiles/test-00.bas";
	Parser p;
	p.load(fname);
	p.parse();

	// streamnode(cout, prog);
	fstream fs("output.txt", ios::out);
	fs << __DATE__ << " -- " << __TIME__ << endl;
	fs << fname << endl;
	fs << "-----" << endl;
	streamnode(fs, p.prog);
	printf("parse OK\n");
}