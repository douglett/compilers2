#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "node.hpp"
using namespace std;


struct Node {
	std::string val;
	std::vector<Node> kids;
};


class Parser {
public:
	// input
	vector<string> lines;
	int lineno = 0;
	Node prog;
	// 
	int load(const string& fname) {
		fstream fs(fname, ios::in);
		if (!fs.is_open())
			return fprintf(stderr, "error: could not open file [%s]\n", fname.c_str()), 1;
		lines = {}, lineno = 0, prog = {};
		string s;
		while (getline(fs, s))
			lines.push_back(s);
		return 0;
	}

	int parse(Node& n) {
		tok_clear_blank();
		// global def block
		n.kids.push_back({ "def_block" });
		parse_def_block(n.kids.back());
		// parse subroutines
		return 0;
	}

private:
	// token management
	int tok_exists() {
		return (lineno < (int)lines.size());
	}
	vector<string> tok_split(const string& str) {
		vector<string> vs;
		stringstream ss(str);
		string s;
		while (ss >> s)  vs.push_back(s);
		return vs;
	}
	vector<string> tok_tokline() {
		if (!tok_exists()) return {};
		return tok_split( lines[lineno] );
	}
	Node tok_rawline() {
		return { "RAW_LINE", {
			{ "<fname>" },
			{ to_string(lineno+1) },
			{ lines[lineno] }
		}};
	}
	void tok_clear_blank() {
		while (tok_exists()) {
			if (tok_tokline().size() > 0)  break;
			lineno++;
		}
	}
	int tok_next() {
		if (tok_exists()) lineno++;
		tok_clear_blank();
		return tok_exists();
	}

	// parsing block
	int parse_def_block(Node& n) {
		int count = 0;
		n.kids.push_back({ "const_list" });
		n.kids.push_back({ "dim_list" });
		auto& const_list = *(n.kids.end()-2);
		auto& dim_list = *(n.kids.end()-1);
		// consts
		while (tok_exists()) {
			auto tok = tok_tokline();
			if (tok[0] != "const")  break;
			// save
			const_list.kids.push_back(tok_rawline());
			count++;
			tok_next();
		}
		// vars
		while (tok_exists()) {
			auto tok = tok_tokline();
			if (tok[0] != "dim")  break;
			// save
			dim_list.kids.push_back(tok_rawline());
			count++;
			tok_next();
		}
		// OK
		return count;
	}
};


void shownode(const Node& n, int indent=0) {
	string pad(indent*2, ' ');
	if (n.kids.size() == 0) {
		printf("%s{ \"%s\" }\n", pad.c_str(), n.val.c_str());
		return;
	}
	printf("%s{ \"%s\"\n", pad.c_str(), n.val.c_str());
	for (auto nn : n.kids)
		shownode(nn, indent+1);
	printf("%s}\n", pad.c_str());
}


struct Node2 {
	string val;
	vector<Node> kids;
};


int main() {
	printf("hello world\n");

	Node prog = { "test prog" };
	Parser p;
	p.load("../dbclass/test.bas");
	p.parse(prog);

	shownode(prog);
}