#pragma once
#include <string>
#include <vector>
#include <sstream>
#include "helpers.hpp"

namespace dbas {
using namespace std;

struct Token {
	string type;
	string val;
};

class SrcLine {
public:
	int lineno = -1;
	string line;
	vector<Token> toklist;

	SrcLine(const string& str, int lno = -1) {
		line = str, lineno = lno;
		auto vs = helpers::split( str );
		for (const auto& s : vs)  toklist.push_back({ "?", s });
	}

	string join() const {
		string s;
		for (int i = 0; i < (int)toklist.size(); i++)
			s += (i > 0 ? " " : "") + toklist[i].val;
		return s;
	}
};

} // end dbas