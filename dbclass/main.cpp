#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "helpers.hpp"
using namespace std;

struct Token {
	string type;
	string val;
};

// class SrcLine {
// public:
// 	int lineno;
// 	string line;
// 	vector<Token> toklist;
// };

class Def {
public:
	vector<string> constlist;
	vector<string> dimlist;
};

class Sub {
public:
	string name;
	Def defblock;
	vector<string> lines;
};

class Parse {
public:
	// input
	vector<string> lines;
	int lineno = 0;
	// structure
	Def defblock;
	vector<Sub> sublist;

	int load(const string& fname) {
		fstream fs(fname, ios::in);
		if (!fs.is_open())
			return fprintf(stderr, "error: could not open file [%s]\n", fname.c_str()), 1;
		lines = {}, lineno = 0;
		string s;
		while (getline(fs, s))
			lines.push_back(s);
		return 0;
	}

	int parse() {
		try {
			printf("--parse start\n");
			Def def;
			Sub sub;
			tok_clear_blank();
			// global def block
			parse_def_block(defblock);
			// parse SUBs
			while (parse_sub_block(sub)) 
				sublist.push_back(sub);
			// force EOF
			if (tok_clear_blank(), tok_exists())
				throw string("expected EOF at line: "+to_string(lineno));
			printf("--parse OK\n");
		}
		catch (const string& err) {
			fprintf(stderr, "parse error: %s\n", err.c_str());
			return 1;
		}
		return 0;
	}

	void show() {
		printf("--defblock--\n");
		for (const auto& c : defblock.constlist)
			printf("  [%s]\n", c.c_str());
		for (const auto& d : defblock.dimlist)
			printf("  [%s]\n", d.c_str());
		printf("--subblock--\n");
		for (const auto& sub : sublist) {
			printf("SUB [%s]\n", sub.name.c_str());
			for (const auto& c : sub.defblock.constlist)
				printf("  [%s]\n", c.c_str());
			for (const auto& d : sub.defblock.dimlist)
				printf("  [%s]\n", d.c_str());
			for (const auto& l : sub.lines)
				printf("    [%s]\n", l.c_str());
		}
	}

private:
	int tok_exists() {
		return (lineno < (int)lines.size());
	}
	void tok_clear_blank() {
		while (tok_exists()) {
			if (tok_tokenizeln().size() > 0)  break;
			lineno++;
		}
	}
	int tok_next() {
		if (tok_exists()) lineno++;
		tok_clear_blank();
		return tok_exists();
	}
	vector<Token> tok_tokenizeln() {
		// tokenize current line
		if (!tok_exists())
			throw string("missing token at line: " +to_string(lineno));
		auto vs = helpers::split( lines[lineno] );
		vector<Token> vt;
		for (const auto& s : vs)  vt.push_back({ "?", s });
		return vt;
	}
	void tok_show_line() {
		printf("	[%d] [%s]\n", lineno+1, lines[lineno].c_str());
	}

	int parse_def_block(Def& def) {
		def = Def();
		int count = 0;
		// consts
		printf("parsing consts\n");
		while (tok_exists()) {
			auto tok = tok_tokenizeln();
			if (tok[0].val != "const")  break;
			// save
			def.constlist.push_back( lines[lineno] );
			count++;
			tok_next();
		}
		// dim
		printf("parsing dim\n");
		while (tok_exists()) {
			auto tok = tok_tokenizeln();
			if (tok[0].val != "dim")  break;
			// save
			def.dimlist.push_back( lines[lineno] );
			tok_show_line();
			count++;
			tok_next();
		}
		return count;
	}

	int parse_sub_block(Sub& sub) {
		sub = Sub();  // reset
		// sub definition
		printf("parsing sub\n");
		if (!tok_exists())  return 0;
		auto tok = tok_tokenizeln();
		if (!(tok.size() == 2 && tok[0].val == "sub"))  return 0;
		// save sub
		tok_show_line();
		sub.name = tok[1].val;
		tok_next();
		// sub def block
		parse_def_block(sub.defblock);
		// sub body (statements)
		printf("parsing sub body\n");
		while (tok_exists()) {
			auto tok = tok_tokenizeln();
			// end sub
			if (tok.size() == 2 && tok[0].val == "end" && tok[1].val == "sub") {
				tok_next();
				return 1;
			}
			// save line
			sub.lines.push_back( lines[lineno] );
			tok_next();
		}
		// early EOF
		throw string("unexpected EOF in sub at: "+to_string(lineno));
	}
};


int main() {
	printf("hello world\n");

	Parse p;
	if (p.load("test.bas")) return 1;
	if (p.parse()) return 1;
	p.show();
}
