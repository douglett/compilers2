#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "helpers.hpp"
// #include "node.hpp"
using namespace std;


struct Node {
	std::string val;
	std::vector<Node> kids;
};

// void shownode(const Node& n, int indent=0) {
// 	string pad(indent*2, ' ');
// 	if (n.kids.size() == 0) {
// 		printf("%s{ \"%s\" }\n", pad.c_str(), n.val.c_str());
// 		return;
// 	}
// 	printf("%s{ \"%s\"\n", pad.c_str(), n.val.c_str());
// 	for (auto nn : n.kids)
// 		shownode(nn, indent+1);
// 	printf("%s}\n", pad.c_str());
// }

void streamnode(ostream& stream, const Node& n, int indent=0) {
	string pad(indent*2, ' ');
	if (n.kids.size() == 0) {
		stream << pad << "{ \"" << n.val << "\" }" << endl;
		return;
	}
	stream << pad << "{ \"" << n.val << "\"" << endl;
	for (auto nn : n.kids)
		streamnode(stream, nn, indent+1);
	stream << pad << "}" << endl;
}


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
		try {
			tok_clear_blank();
			// global def block
			n.kids.push_back({ "def_block" });
			parse_def_block(n.kids.back());
			// parse subroutines
			n.kids.push_back({ "subroutines" });
			Node subnode;
			while (parse_subroutine(subnode)) 
				n.kids.back().kids.push_back(subnode);
			return 0;
		}
		catch (const string& err) {
			fprintf(stderr, "error: %s\n", err.c_str());
			streamnode(cerr, tok_rawline());
			return 1;
		}
	}

private:
	// token management
	int tok_exists() {
		return (lineno < (int)lines.size());
	}
	vector<string> tok_tokline() {
		if (!tok_exists()) return {};
		return helpers::split( lines[lineno] );
	}
	Node tok_rawline() {
		return { "RAW_LINE", {
			{ "<fname>" },
			{ to_string(lineno+1) },
			{ helpers::chomp(lines[lineno]) }
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
		// consts
		n.kids.push_back({ "const_list" });
		auto& const_list = n.kids.back();
		while (tok_exists()) {
			auto tok = tok_tokline();
			if (tok[0] != "const")  break;
			// save
			const_list.kids.push_back(tok_rawline());
			count++;
			tok_next();
		}
		// vars
		n.kids.push_back({ "dim_list" });
		auto& dim_list = n.kids.back();
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

	int parse_subroutine(Node& n) {
		// check for valid sub
		if (!tok_exists()) return 0;
		auto tok = tok_tokline();
		if (tok[0] != "sub") throw string("expected sub");
		if (tok.size() != 2) throw string("expected sub name");
		// initialize sub node
		string name = tok[1];
		n = { name };
		tok_next();
		// sub contents
		while (tok_exists()) {
			tok = tok_tokline();
			// check for end of sub
			if (tok.size() == 2 && tok[0] == "end" && tok[1] == "sub")
				return tok_next(), 1;
			// sub contents
			n.kids.push_back(tok_rawline());
			tok_next();
		}
		// early EOF before end of block
		throw string("unexpected EOF in sub ["+name+"]");
	}
};


int main() {
	printf("hello world\n");

	Node prog = { "test prog" };
	Parser p;
	p.load("../dbclass/test.bas");
	p.parse(prog);

	// streamnode(cout, prog);
	fstream fs("output.txt", ios::out);
	fs << __DATE__ << " -- " << __TIME__ << endl << "-----" << endl;
	streamnode(fs, prog);
	printf("parse OK\n");
}