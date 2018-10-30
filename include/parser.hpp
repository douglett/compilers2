#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "helpers.hpp"
#include "node.hpp"
#include "parser_structure.hpp"

namespace dbas {
using namespace std;

class Parser {
public:
	// vars
	string fname;
	vector<string> lines;
	Node prog;
	// 
	int load(const string& fname) {
		fstream fs(fname, ios::in);
		if (!fs.is_open())
			return fprintf(stderr, "error: could not open file [%s]\n", fname.c_str()), 1;
		this->fname = fname, prog = {}, lines = {};
		string s;
		while (getline(fs, s))
			lines.push_back(s);
		return 0;
	}

	int parse() {
		Parser_Structure p;
		try {
			p.parse(fname, lines);
			prog = p.prog;
			return 0;
		}
		catch (const string& err) {
			fprintf(stderr, "error: %s\n", err.c_str());
			streamnode(cerr, p.tok_rawline());
			return 1;
		}
	}
};

};